#include "pch.h"
#include "TileManager.h"
#include "Core/EngineCore.h"
#include "Heap.h"


// Direction vectors
const int dRow[] = { -1, 0, 1, 0 };
const int dCol[] = { 0, 1, 0, -1 };
const int maxSteps = 1000;
TileManager::TileManager(Frac::EngineConnector* connector) :  m_connector(connector),
m_runffq{ m_connector->GetEngine().GetWorld().makeQuery<BaseTag, GridPositionComponent>()},
m_gridTileq{ m_connector->GetEngine().GetWorld().makeQuery<GridPositionComponent, TileComponent>() },
m_tileq{ m_connector->GetEngine().GetWorld().makeQuery<TileComponent>() }
{
    LOGINFO("Tile Manager Initialized");
}

TileManager::~TileManager()
{
    m_connector = nullptr;
}

void TileManager::CreateTileMap(uint rows, uint columns, std::vector<std::vector<int>>* gridData)
{
	m_tileMap = std::make_unique<TileMap>(rows, columns, m_connector);
	m_tileMap->CreateTiles(gridData);
    //SetTileNeighbors();
}

void TileManager::RunFloodFillAlgorithm()
{
    LOGINFO("Flood Fill Started");

    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

    const uint allRows = m_tileMap->m_rows;
    const uint allCols = m_tileMap->m_cols;

    //Find our Base
    glm::vec2 baseCoords;
    std::function<void(Frac::It, uint64_t, BaseTag*, GridPositionComponent*)> func = [&baseCoords](Frac::It i, uint64_t index, BaseTag* t, GridPositionComponent* p)
    {
        baseCoords = p[index].m_gridPosition;
    };
    m_runffq.Iter(func);

   

    uint curRow = (uint)baseCoords.x;
    uint curCol = (uint)baseCoords.y;
    uint stepsRow = 1;
    uint stepsCol = 1;
    uint curStepsRow = stepsRow + curRow;
    uint curStepsCol = stepsCol + curCol;
    uint indexCount = 0;
    bool isGridFilled = false;

    size_t baseID = (size_t)(baseCoords.y + baseCoords.x * allCols);
    world.get<TileComponent>(m_tileMap->m_tileEntities->at(baseID))->m_states.m_didCheck = true;
    world.get<TileComponent>(m_tileMap->m_tileEntities->at(baseID))->m_floodFill.m_floodFillCost = indexCount;
    
    while (isGridFilled == false)
    {
        for (uint x = curRow; x < curStepsRow; x++) //check if less than the Array
        {
            for (uint y = curCol; y < curStepsCol; y++)
            {
                size_t curTileID = (size_t)(y + x * allCols);

                const TileComponent* curTile = world.get<TileComponent>(m_tileMap->m_tileEntities->at(curTileID));

                const bool isWalkable = curTile->m_states.m_isWalkable;
                const bool didCheck = curTile->m_states.m_didCheck;
                const uint floodCost = curTile->m_floodFill.m_floodFillCost;

                //current Tile
                if ((isWalkable) && (didCheck) && (floodCost == indexCount))
                {
                    // Check neighbors
                    for (uint a = 0; a < 4; a++)
                    {
                        uint adjx = x + dRow[a];
                        uint adjy = y + dCol[a];

                        size_t adjID = (size_t)(adjy + adjx * allCols);

                        if (adjID < (size_t)allRows * allCols)
                        {
                            TileComponent* nextTile = world.get<TileComponent>(m_tileMap->m_tileEntities->at(adjID));

                            if ((nextTile->m_states.m_isActive) && (nextTile->m_states.m_didCheck == false) && (nextTile->m_states.m_isWalkable) && IsInGrid(glm::vec2(adjx, adjy)))
                            {
                                nextTile->m_states.m_didCheck = true;
                                nextTile->m_floodFill.m_floodFillCost = indexCount + 1;
                            }
                        }
                    }
                }
            }
        }

        indexCount++;
        stepsRow += 2;
        stepsCol += 2;
        if (stepsRow > allRows) stepsRow = allRows;
        if (stepsCol > allCols) stepsCol = allCols;

        if (curRow != 0) curRow--;
        if (curCol != 0) curCol--;

        curStepsRow = stepsRow + curRow;
        curStepsCol = stepsCol + curCol;

        if (curStepsRow > allRows) curStepsRow = allRows;
        if (curStepsCol > allCols) curStepsCol = allCols;

        //Stops when all Tiles have been reached
        isGridFilled = DidGridFill();     
        if (indexCount >= maxSteps)
        {
            LOGWARNING("Flood Fill Cannot Reach Target");
            isGridFilled = true;
        }
    }

    LOGINFO("Flood Fill Completed");
    //m_tileMap->PrintTileInfo();
    //m_tileMap->PrintGrid();
}

void TileManager::ResetFloodFillCost()
{
    std::function<void(Frac::It, uint64_t, TileComponent*)> func = [](Frac::It i, uint64_t index, TileComponent* c)
    {
        if ((c[index].m_states.m_isActive == true) && (c[index].m_floodFill.m_floodFillCost != 9999))
        {
            c[index].m_floodFill.m_floodFillCost = 9999;
        }
    };
    m_tileq.Iter(func);
}

void TileManager::RunAStarALgorithm(glm::vec2 gridStartPosition, glm::vec2 gridTargetPosition)
{
    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
    EntityHandle StartTileEH = m_tileMap->m_tileEntities->at(GetTileID(gridStartPosition));
    EntityHandle EndTileEH = m_tileMap->m_tileEntities->at(GetTileID(gridTargetPosition));
    world.get_mutable<BoxComponent>(StartTileEH)->color = glm::vec4{ 1, 0, 0, 1 };
    world.get_mutable<BoxComponent>(EndTileEH)->color = glm::vec4{ 1, 0, 0, 1 };

    std::vector<EntityHandle> openSet;
    std::unordered_set<EntityHandle> closedSet;
    openSet.push_back(StartTileEH);

    while (openSet.size() > 0) {
        EntityHandle node = openSet[0];
        TileComponent* nodeTC = world.get_mutable<TileComponent>(node);
        GridPositionComponent* nodeGPC = world.get_mutable<GridPositionComponent>(node);
        for (int i = 1; i < openSet.size(); i++) {
            //EntityHandle* nodeIEH = openSet[i];
            TileComponent* nodeITC = world.get_mutable<TileComponent>(openSet[i]);
            if (nodeITC->m_aStar.GetFCost() < nodeTC->m_aStar.GetFCost() || nodeITC->m_aStar.GetFCost() == nodeTC->m_aStar.GetFCost()) {
                if (nodeITC->m_aStar.m_hCost < nodeTC->m_aStar.m_hCost)
                    node = openSet[i];
                   
            }
        }
        openSet.erase(std::remove(openSet.begin(), openSet.end(), node), openSet.end());
        closedSet.insert(node);

        nodeTC = world.get_mutable<TileComponent>(node);
        nodeGPC = world.get_mutable<GridPositionComponent>(node);

        if ((node) == (EndTileEH)) {
            RetracePath(StartTileEH, EndTileEH);
            std::cout << "finsihed and found" << std::endl;
            return;
        }

        for (int i = 0; i < nodeTC->m_aStar.m_SideTiles.size(); i++)
        {
            TileComponent* neighbourTC = world.get_mutable<TileComponent>(nodeTC->m_aStar.m_SideTiles[i]);
            if (!neighbourTC->m_states.m_isWalkable || (closedSet.find(nodeTC->m_aStar.m_SideTiles[i]) != closedSet.end()) == true) { 
                continue;
            }

            int newCostToNeighbour = nodeTC->m_aStar.m_gCost + GetDistance(node, nodeTC->m_aStar.m_SideTiles[i]);
            if (newCostToNeighbour < neighbourTC->m_aStar.m_gCost || (std::find(openSet.begin(), openSet.end(), nodeTC->m_aStar.m_SideTiles[i]) != openSet.end()) == false) {
                neighbourTC->m_aStar.m_gCost = newCostToNeighbour;
                neighbourTC->m_aStar.m_hCost = GetDistance(nodeTC->m_aStar.m_SideTiles[i], EndTileEH);
                neighbourTC->m_aStar.m_parentEH = node;

                if ((std::find(openSet.begin(), openSet.end(), nodeTC->m_aStar.m_SideTiles[i]) != openSet.end()) == false) {
                    openSet.push_back(nodeTC->m_aStar.m_SideTiles[i]);
               }
            }
        }
    }
}

void TileManager::RetracePath(EntityHandle startEH, EntityHandle targetEH) {
    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

    m_LastRetracedPath.clear();
    EntityHandle currentNode = targetEH;

    while (currentNode != startEH) {
        m_LastRetracedPath.push_back(currentNode);
        GridPositionComponent* newcheck = world.get_mutable<GridPositionComponent>(currentNode);
        TileComponent* currentNodeTC = world.get_mutable<TileComponent>(currentNode);
        currentNode = currentNodeTC->m_aStar.m_parentEH;
    }
    std::reverse(m_LastRetracedPath.begin(), m_LastRetracedPath.end());

    for (unsigned int i = 0; i < m_LastRetracedPath.size(); i++) {
        world.get_mutable<BoxComponent>(m_LastRetracedPath[i])->color = glm::vec4{ 1, 0, 0, 1 };
    }
}

void TileManager::SetTileOccupancy(glm::vec2 gridPos, bool occupy, EntityHandle e)
{
    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

    const size_t curTileID = GetTileID(gridPos);
    TileComponent* curTile = world.get<TileComponent>(m_tileMap->m_tileEntities->at(curTileID));

    if (curTile->m_floodFill.m_floodFillCost != 0)
    {
        if ((occupy) && (curTile->m_occupier == (uint64_t)0))
        {
            curTile->m_occupier = e;
            world.get_mutable<BoxComponent>(m_tileMap->m_tileEntities->at(curTileID))->color = glm::vec4{ 0.5, 1, 0, 1 };
        }
        else if (!occupy)
        {
            curTile->m_occupier = (uint64_t)0;
            world.get_mutable<BoxComponent>(m_tileMap->m_tileEntities->at(curTileID))->color = glm::vec4(1);
        }
    }
}

void TileManager::SetTileLocked(glm::vec2 gridPos, bool lock)
{
    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

    const size_t curTileID = GetTileID(gridPos);
    TileComponent* curTile = world.get<TileComponent>(m_tileMap->m_tileEntities->at(curTileID));

    if (curTile->m_floodFill.m_floodFillCost != 0 && !IsTileOccupied(gridPos))
    {
        if (lock)
        {
            curTile->m_states.m_isLocked = true;
            const TileComponent* curTile2 = world.get<TileComponent>(m_tileMap->m_tileEntities->at(curTileID));
            int a = 5;
            world.get_mutable<BoxComponent>(m_tileMap->m_tileEntities->at(curTileID))->color = glm::vec4{ 1, 0.5, 0, 1 };
        }
        else if (!lock)
        {
            curTile->m_states.m_isLocked = false;
            world.get_mutable<BoxComponent>(m_tileMap->m_tileEntities->at(curTileID))->color = glm::vec4(1);
        }
        
    }
}

void TileManager::SetTileAvailability(glm::vec2 gridPos, bool isAvailable, EntityHandle e)
{

    SetTileOccupancy(gridPos, !isAvailable, e);
    SetTileLocked(gridPos, !isAvailable);
    
}

bool TileManager::IsTileOccupied(glm::vec2 gridPos)
{
    if (IsInGrid(gridPos))
    {
        Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
        const size_t curTileID = GetTileID(gridPos);
        const TileComponent* curTile = world.get<TileComponent>(m_tileMap->m_tileEntities->at(curTileID));

        if ((curTile->m_occupier == (uint64_t)0 || curTile->m_floodFill.m_floodFillCost == 0) && curTile->m_states.m_isActive)
        {
            return false;
        }

        return true;
    }
    LOGERROR("Tried to access Invalid Tile for isOccupied. Pos: %f , %f", gridPos.x, gridPos.y);
    return true;
}

bool TileManager::IsTileLocked(glm::vec2 gridPos)
{
    if (IsInGrid(gridPos))
    {
        Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
        const size_t curTileID = GetTileID(gridPos);
        const TileComponent* curTile = world.get<TileComponent>(m_tileMap->m_tileEntities->at(curTileID));

        if (curTile->m_states.m_isLocked == false || curTile->m_floodFill.m_floodFillCost == 0)
        {
            return false;
        }

        return true;
    }
    LOGERROR("Tried to access Invalid Tile for isLocked. Pos: %f , %f", gridPos.x, gridPos.y);
    return true;
}

bool TileManager::CanMoveIntoTile(glm::vec2 gridPos)
{
    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

    if (IsTileLocked(gridPos) || IsTileOccupied(gridPos))
    {
        return false;
    }

    return true;
}

glm::vec2 TileManager::GetNextTilePos(size_t tileID, uint patience)
{
    const uint allRows = m_tileMap->m_rows;
    const uint allCols = m_tileMap->m_cols;

    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

    const uint curCost = world.get<TileComponent>(m_tileMap->m_tileEntities->at(tileID))->m_floodFill.m_floodFillCost;
    const glm::vec2 curPos = world.get<GridPositionComponent>(m_tileMap->m_tileEntities->at(tileID))->m_gridPosition;

    uint bestCost = curCost;
    glm::vec2 bestPosition = curPos;
    
    for (uint i = 0; i < 4; i++)
    {
        uint adjx = curPos.x + dRow[i];
        uint adjy = curPos.y + dCol[i];

        size_t adjID = (size_t)(adjy + adjx * allCols);

        if (adjID < (size_t)allRows * allCols)
        {
            TileComponent* nextTile = world.get<TileComponent>(m_tileMap->m_tileEntities->at(adjID));
            glm::vec2 nextPos = world.get<GridPositionComponent>(m_tileMap->m_tileEntities->at(adjID))->m_gridPosition;

            const bool isWalkable = nextTile->m_states.m_isWalkable;
            const bool isActive = nextTile->m_states.m_isActive;
            const uint floodCost = nextTile->m_floodFill.m_floodFillCost;

            if ((isActive) && (isWalkable) && CanMoveIntoTile(nextPos))
            {
                if (floodCost <= bestCost - patience)
                {
                    bestCost = floodCost;
                    bestPosition = nextPos;
                }
            } 
        }
    }
    return bestPosition;
}

const size_t TileManager::GetTileID(const glm::vec2 enemyGridPos)
{
    if (IsInGrid(enemyGridPos))
    {
        return (enemyGridPos.y + enemyGridPos.x * m_tileMap->m_cols);
    }
    LOGERROR("Tried to access Invalid Tile ID. Pos: %f , %f", enemyGridPos.x, enemyGridPos.y);
    return -1;
  
}

bool TileManager::DidGridFill()
{
    bool result = true;

    std::function<void(Frac::It, uint64_t, TileComponent*)> func = [&result](Frac::It i, uint64_t index, TileComponent* c)
    {
        if ((c[index].m_states.m_isActive == true) && (c[index].m_floodFill.m_floodFillCost == 9999) && (c[index].m_states.m_isWalkable == true))
        {
            result = false;
            return;
        }       
    };

    m_tileq.Iter(func);

    return result;

}

bool TileManager::IsInGrid(glm::vec2 gridPos)
{
    const uint rows = m_tileMap->m_rows;
    const uint cols = m_tileMap->m_cols;

    if ((gridPos.x < 0) || (gridPos.x >= rows) || (gridPos.y < 0) || (gridPos.y >= cols))
    {
        return false;
    }
    return true;
}

void TileManager::SetTileNeighbors() {
    const int allRows = m_tileMap->m_rows -1;
    const int allCols = m_tileMap->m_cols -1;
    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
 
    std::function<void(Frac::It, uint64_t, GridPositionComponent*, TileComponent*)> func = [allRows,allCols, this, &world](Frac::It i, uint64_t index, GridPositionComponent*gridc, TileComponent* tilec)
    {
        int amount = 0;
        //std::cout << "Active tile: " + index << std::endl;
        tilec[index].m_aStar.m_SideTiles.clear();
        if (tilec[index].m_states.m_isActive) {
            //std::cout << "Origional x:" << gridc[index].m_gridPosition.x << " Origional y:" << gridc[index].m_gridPosition.y << std::endl;
            tilec[index].m_aStar.m_SideTiles.clear();
            for (int x = gridc[index].m_gridPosition.x-1; x <= gridc[index].m_gridPosition.x + 1; x++) {
                for (int y = gridc[index].m_gridPosition.y -1; y <= gridc[index].m_gridPosition.y + 1; y++)
                {
                    if (x != gridc[index].m_gridPosition.x || y != gridc[index].m_gridPosition.y)
                    {
                        if (x >= 0 && x <= allRows) {
                            if (y >= 0 && y <= allCols) {
                                //std::cout << "neighbor x:" << x << " neighbor y:" << y << std::endl;
                                const TileComponent* neighborTilec = world.get<TileComponent>(m_tileMap->GetTileHandle(glm::vec2(x, y)));
                                if (neighborTilec->m_states.m_isActive) {
                                    tilec[index].m_aStar.m_SideTiles.push_back(m_tileMap->GetTileHandle(glm::vec2(x, y)));
                                }
                            }
                        }
                    }
                    
                }
            }
   
        }
    };
    m_gridTileq.Iter(func);
}

int TileManager::GetDistance(EntityHandle tileA, EntityHandle tileB) {
    Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
    GridPositionComponent* a = world.get_mutable<GridPositionComponent>(tileA);
    GridPositionComponent* b = world.get_mutable<GridPositionComponent>(tileB);

    int dstX = glm::abs(a->m_gridPosition.x - b->m_gridPosition.x);
    int dstY = glm::abs(a->m_gridPosition.y - b->m_gridPosition.y);

    if (dstX > dstY) {
        return 14 * dstY + 10 * (dstX - dstY);
    }
    else {
        return 14 * dstX + 10 * (dstY - dstX);
    }
}



