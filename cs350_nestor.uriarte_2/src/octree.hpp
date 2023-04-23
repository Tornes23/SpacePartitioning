/**
* @file		 octree.hpp      
* @author	 Nestor Uriarte,  nestor.uriarte@digipen.edu    
* @date		 Sat Nov  7 22:16:50 2020          
* @brief	 Contains the definition of the Octree class     
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#ifndef CS350_OCTREE_TEST_OCTREE_HPP
#define CS350_OCTREE_TEST_OCTREE_HPP
#include "geometry.hpp"

namespace cs350 {

    template <int dimension = 3>
    uint32_t compute_locational_code(glm::vec<dimension, int> world_position, uint32_t root_size, uint32_t levels);
    uint32_t compute_locational_code(aabb const& bv, uint32_t root_size, uint32_t levels);
    aabb     compute_bv(uint32_t locational_code, uint32_t root_size);
    uint32_t locational_code_depth(uint32_t lc);
    uint32_t common_locational_code(uint32_t lc1, uint32_t lc2);

    /**
     * @brief
     * 	Linear octree, each node stores a head for a linked list of T
     * @tparam T
     */
    template <typename T>
    class octree
    {
      public:
        struct node
        {
            node(uint32_t loc = 0b1);
            uint32_t locational_code; 
            uint8_t  children_active;
            T*       first;
        };

      private:
        std::unordered_map<uint32_t, node*> m_nodes;
        uint32_t                            m_root_size;
        uint32_t                            m_levels;

      public:
        octree();
        ~octree();
        void        Initialize(uint32_t size, uint32_t levels);
        void        destroy();
        node*       find_create_node(aabb const& bv);
        node*       find_node(aabb const& bv);
        node const* find_node(aabb const& bv) const;
        node*       find_create_node(uint32_t locational_code);
        node*       find_node(uint32_t locational_code);
        node const* find_node(uint32_t locational_code) const;
        void        delete_node(uint32_t locational_code);

        void        GetNodesOfLevel(uint32_t level, std::vector<node*>& container);

        [[nodiscard]] uint32_t root_size() const { return m_root_size; }
        void                   set_root_size(uint32_t size) { m_root_size = size; }
        void                   set_levels(uint32_t levels) { m_levels = levels; }
    };
    
}

#include "octree.inl" 

#endif //CS350_OCTREE_TEST_OCTREE_HPP
