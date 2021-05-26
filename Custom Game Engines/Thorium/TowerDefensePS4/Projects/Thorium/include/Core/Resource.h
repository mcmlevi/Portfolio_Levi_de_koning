#include <string>

namespace Th
{
	class ResourceManager;
	/**
	 * \brief Resource type available, such as Textures, Sounds, JSON these could also be used as flags for easy testing.
	 */
	enum class eResourceType
	{
		ResTexture = 1 << 0,
		ResSound = 1 << 1,
		ResJSON = 1 << 2,
		ResShader = 1 << 3,
		ResMesh = 1 << 4,
	};

	class Resource
	{
	
	public:
		/**
		* \brief Resource Constructor
		* \param filepath to the resource
		* \param type what type of resource this is.
		*/
		Resource(const std::string& filepath, const eResourceType type);
		/**
		 * \brief virtual destructor to delete the resource
		 */
		virtual ~Resource() = default;
		/**
		 * \brief Get the unique resource ID
		 * \return resource ID
		 */
		std::string GetID();
		/**
		 * \brief Get the type of the object
		 * \return Object type
		 */
		eResourceType GetType();
	protected:


		
		
		std::string m_resourcePath{};
		eResourceType m_type{};
		
	private:

		
	};
}
