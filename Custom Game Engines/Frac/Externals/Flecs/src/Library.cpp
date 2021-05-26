#include <Flecs\flecs.h>
#include "Flecs\TestSystem.h"
#include <iostream>

struct Position {
    float x;
    float y;
};

struct customEnginePhase {

};

void Action(flecs::iter& it) {
    std::cout << it.system().name() << " called!" << std::endl;
}

//int main(int argc, char* argv[]) {
//    TestSystem test;
//    /* Create the world, pass arguments for overriding the number of threads,fps
//    * or for starting the admin dashboard (see flecs.h for details). */
//    flecs::world ecs(argc, argv);
//
//    auto a = ecs.component<Velocity>();
//
//
//
//
//    auto SystemA = ecs.system<Velocity>("SystemA").kind(flecs::OnUpdate).each(test.SystemFunction);
//
//
//
//    ecs.entity("Test1", 10)
//        .set<Velocity>({ 5,5 });
//    ecs.entity("Test2", 10)
//        .set<Velocity>({ 15,5 });
//
//    ecs.entity("Test3", 10)
//        .set<Velocity>({ 1,5 });
//    ecs.entity("Test4", 10)
//        .set<Velocity>({ 12,5 });
//    ecs.entity("Test5", 10)
//        .set<Velocity>({ 15,5 });
//
//    /* Create dummy systems */
//
//    /* auto SystemB = ecs.system<>("SystemB").action(Action);
//     auto SystemC = ecs.system<>("SystemC").action(Action);
//     auto SystemD = ecs.system<>("SystemD").action(Action);
//     auto SystemE = ecs.system<>("SystemE").action(Action);*/
//
//     /* Create two features, each with a set of systems. Features are regular
//      * types, and the name feature is just a convention to indicate that a type
//      * only contains systems. Since systems, just like components, are stored as
//      * entities, they can be contained by types. */
//    auto Feature1 = ecs.type("Feature1")
//        .add(SystemA);
//    //.add(SystemB);
//
///*auto Feature2 = ecs.type("Feature2")
//    .add(SystemC)
//    .add(SystemD);*/
//
//    /* Create a feature that includes Feature2 and SystemE. Types/features can
//     * be organized in hierarchies */
//     /* auto Feature3 = ecs.type("Feature3")
//          .add(Feature2)
//          .add(SystemE);*/
//
//          /* First, disable Feature1 and Feature3. No systems will be executed. */
//          //std::cout << "Disable Feature1, Feature3" << std::endl;
//          //Feature1.disable();
//          ////Feature3.disable();
//          //ecs.progress();
//
//          /* Enable Feature3. SystemC, SystemD and SystemE will be executed */
//          /*std::cout << std::endl << "Enable Feature3" << std::endl;
//          Feature3.enable();
//          ecs.progress();*/
//
//          /* Disable Feature2. SystemE will be executed */
//         /* std::cout << std::endl << "Disable Feature2" << std::endl;
//          Feature2.disable();
//          ecs.progress();*/
//
//          /* Enable Feature1. SystemA, SystemB and SystemE will be executed. */
//    std::cout << std::endl << "Enable Feature1" << std::endl;
//    Feature1.enable();
//    ecs.progress();
//    SystemA.disable();
//    bool testA = SystemA.enabled();
//    ecs.progress();
//    SystemA.enable();
//    bool testB = SystemA.enabled();
//
//    ecs.progress();
//    /* Disable only SystemE */
//   /* std::cout << std::endl << "Disable SystemE" << std::endl;
//    SystemE.disable();
//    ecs.progress();*/
//}