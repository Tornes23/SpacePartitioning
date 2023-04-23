/**
* @file		 octree.inl      
* @author	 Nestor Uriarte,  nestor.uriarte@digipen.edu    
* @date		 Sat Nov  7 22:16:07 2020          
* @brief	 Contains te implementation of the template functions on the octree class     
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "octree.hpp"

namespace cs350
{
/**
* @brief	Computes the locational code based on a world position
* @param	glm::vec<dimension, int> world_position
* @param	uint32_t root_size
* @param	uint32_t levels
* @return	uint32_t
**/
    template <int dimension>
    uint32_t compute_locational_code(glm::vec<dimension, int> world_position, uint32_t root_size, uint32_t levels)
    {
        //variable to store the code
        uint32_t code = 1;
        
        //vector to store the position adapted to the octree coordinate system
        glm::vec<dimension, int> offsetted;

        //changin it to the octree coord system
        for(int i = 0; i < dimension; i++)
        { 
            offsetted[i] = world_position[i] + (root_size / 2);
        }

        //if is outside return that is in the root
        for (int i = 0; i < dimension; i++)
        {
            if (offsetted[i] >= static_cast<int>(root_size) || offsetted[i] < 0 )
                return 1;
        }

        //getting the starting bit index to check
        uint32_t StartBitPos = glm::log2(root_size) - 1;

        //for each level the tree has
        for (uint32_t i = levels; i > 0; i--)
        {
            //right shift to insert the code
            code <<= dimension;

            //the coord to check
            int coord = 1;

            //for each dimension
            for (int j = dimension - 1; j >= 0; j--)
            {
                //getting if above or below the checking bit
                if (offsetted[j] & (1 << StartBitPos))
                {
                    //in which position to set the bit active
                    int pos = dimension - coord;

                    //appending it to the code
                    code = code | (1 << pos);
                }

                //incrementing the counter
                coord++;
            }

            //going one bit to the left
            StartBitPos--;
        }

        //returning the code
        return code;
    }

/**
* @brief	The constructor of the class
* @param	uint32_t loc
**/
    template<typename T>
    octree<T>::node::node(uint32_t loc)
    {
        //setting the code
        locational_code = loc;

        //setting the active childs as 0
        children_active = 0b0;

        //setting the pointer to null
        first = nullptr;
    }

/**
* @brief	The constructor of the class
**/
    template<typename T>
    octree<T>::octree()
    {
        //initializing the root size and levels to 0
        m_root_size = 0;
        m_levels = 0;
    }
    
/**
* @brief	The destructor of the class
**/
    template<typename T>
    octree<T>::~octree()
    {
        //calling to destroy
        destroy();
    }
    
/**
* @brief	Initializes the octree
* @param	uint32_t size
* @param	uint32_t levels
* @return	void
**/
    template<typename T>
    void octree<T>::Initialize(uint32_t size, uint32_t levels)
    {
        //creating a new node
        node* new_node = find_create_node(0b1);

        //if is valid
        if (new_node == nullptr)
            m_nodes[0b1] = new_node;

        //setting the variables
        m_levels = levels;
        m_root_size = size;
    }
    
/**
* @brief	Destroys the octree
* @return	void
**/
    template<typename T>
    void octree<T>::destroy()
    {
        //for each node delete it
        for (auto it : m_nodes)
        {
            delete it.second;
        }

        //clear the map
        m_nodes.clear();
    }
    
/**
* @brief	Finds or creates a node based on an aabb
* @param	aabb const& bv
* @return	typename octree<T>::node*
**/
    template<typename T>
    typename octree<T>::node* octree<T>::find_create_node(aabb const& bv)
    {
        //getting the loactional code for the aabb
        uint32_t locational_code = compute_locational_code(bv, m_root_size, m_levels);

        //if is higher than the maximum level
        if (locational_code_depth(locational_code) > m_levels)
            return nullptr;

        //if the node is alredy on the map
        auto found = m_nodes.find(locational_code);

        //pointer to store the wanted node
        node* wanted = nullptr;

        //if it was not found on the map
        if (found == m_nodes.end())
        {
            //creating a new node
            wanted = new node(locational_code);

            //asserting is valid
            assert(wanted != nullptr);

            //getting the depth of the code
            uint32_t depth = locational_code_depth(locational_code);

            //if is not the root
            if (depth != 1)
            {
                //get the parent code
                uint32_t parentCode = locational_code >> 3;

                //get the child last 3 bits
                uint32_t childCode = locational_code & 0b111;

                //getting the parent node
                node* parent = find_create_node(parentCode);

                //setting the current children as active
                parent->children_active |= (1 << childCode);
            }

            //inserting it into the map
            m_nodes[locational_code] = wanted;

        }
        else//if it was found set as the one stored
            wanted = found->second;

        //return the node pointer
        return wanted;
    }
    
/**
* @brief	Finds a node based on an aabb
* @param	aabb const& bv
* @return	typename octree<T>::node*
**/
    template<typename T>
    typename octree<T>::node* octree<T>::find_node(aabb const& bv)
    {
        //getting the locational code
        uint32_t code = compute_locational_code(bv);

        //returning the node at that position
        return find_node(code);
    }

/**
* @brief	Finds a node based on an aabb
* @param	aabb const& bv
* @return	typename octree<T>::node const*
**/
    template<typename T>
    typename octree<T>::node const* octree<T>::find_node(aabb const& bv) const
    {
        //getting the locational code
        uint32_t code = compute_locational_code(bv);

        //returning the node at that position
        return find_node(code);
    }

/**
* @brief	Finds or creates a node based on a locational code
* @param	uint32_t locational_code
* @return	typename octree<T>::node*
**/
    template<typename T>
    typename octree<T>::node* octree<T>::find_create_node(uint32_t locational_code)
    {
        //checking the code is valid
        if (locational_code == 0 || locational_code_depth(locational_code) > m_levels)
            return nullptr;

        //if the node is alredy on the map
        auto found = m_nodes.find(locational_code);

        //pointer to store the wanted node
        node* wanted = nullptr;

        //if it was not found on the map
        if (found == m_nodes.end())
        {
            //creating a new node
            wanted = new node(locational_code);

            //asserting is valid
            assert(wanted != nullptr);

            //getting the depth of the code
            uint32_t depth = locational_code_depth(locational_code);

            //if is not the root
            if (depth != 0)
            {
                //get the parent code
                uint32_t parentCode = locational_code >> 3;

                //get the child last 3 bits
                uint32_t childCode = locational_code & 0b111;

                //getting the parent node
                node* parent = find_create_node(parentCode);

                //setting the current children as active
                parent->children_active |= (1 << childCode);
            }

            //inserting it into the map
            m_nodes[locational_code] = wanted;

        }
        else//if it was found set as the one stored
            wanted = found->second;

        //return the node pointer
        return wanted;
    }

/**
* @brief	Finds a node based on a locational code
* @param	uint32_t locational_code
* @return	typename octree<T>::node*
**/
    template<typename T>
    typename octree<T>::node* octree<T>::find_node(uint32_t locational_code)
    {
        //pointer to store the wanted node
        node* wanted = nullptr;

        //if it is found
        auto found = m_nodes.find(locational_code);

        if (found != m_nodes.end())
        {
            //set the pointer
            wanted = found->second;
        }

        //returning the pointer
        return wanted;
    }

/**
* @brief	Finds a node based on a locational code
* @param	uint32_t locational_code
* @return	typename octree<T>::node const*
**/
    template<typename T>
    typename octree<T>::node const* octree<T>::find_node(uint32_t locational_code) const
    {
        //pointer to store the wanted node
        const node* wanted = nullptr;

        //if it is found
        auto found = m_nodes.find(locational_code);

        if (found != m_nodes.end())
        {
            //set the pointer
            wanted = found.second;
        }

        //returning the pointer
        return wanted;
    }
    
/**
* @brief	Deletes a node with the given locational code
* @param	uint32_t locational_code
* @return	void
**/
    template<typename T>
    void cs350::octree<T>::delete_node(uint32_t locational_code)
    {
        //if is the root return
        if (locational_code == 0b1)
            return;

        //get the node and its childs and go deleting 
        uint32_t parentCode = locational_code >> 3;

        //getting the parent node
        node* parent = find_node(parentCode);

        //getting the las 3 bits for the child code
        uint32_t childCode = locational_code & 0b111;

        //deactivating the child bit
        parent->children_active &= ~(1 << childCode);
         
        //deleting the pointer
        delete m_nodes[locational_code];

        //setting it as nullptr
        m_nodes[locational_code] = nullptr;

        //erasing it from the map
        m_nodes.erase(locational_code);

        //if the parent is also empty recursive call to delete it
        if (parent->first == nullptr && parent->children_active == 0)
            delete_node(parentCode);

    }
    
/**
* @brief	Gets all the nodes of the wanted level
* @param	uint32_t level
* @param	std::vector<node*>& container
* @return	void
**/
    template<typename T>
    void octree<T>::GetNodesOfLevel(uint32_t level, std::vector<node*>& container)
    {
        //for each node on the map
        for (auto it : m_nodes)
        {
            //if the depth of the code is the one we want insert it to the container
            if (locational_code_depth(it.first) == level)
                container.push_back(it.second);
        }
    }

}