/**
* @file		 octree.cpp      
* @author	 Nestor,  nestor.uriarte@digipen.edu    
* @date		 Sat Nov  7 22:19:05 2020          
* @brief	 Contains the implementation of the functions on the Octree class     
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include "octree.hpp"

namespace cs350 {
    
/**
* @brief	COmputes a locational code based on an aabb, the octree root size and the amount of levels it can have
* @param	aabb const& bv
* @param	uint32_t root_size
* @param	uint32_t levels
* @return	    uint32_t
**/
    uint32_t compute_locational_code(aabb const& bv, uint32_t root_size, uint32_t levels)
    {
        //vectors to store the floored and ceiled positions of the min and max od the bv
        glm::vec<3, int> min;
        glm::vec<3, int> max;

        //applying the floor and ceiling to the coordinates
        for (int i = 0; i < 3; i++)
        {
            min[i] = static_cast<int>(glm::floor(bv.mMin[i]));
            max[i] = static_cast<int>(glm::ceil(bv.mMax[i]));
        }

        //getting the code for the mion point and the max point
        uint32_t codeMin = compute_locational_code<3>(min, root_size, levels);
        uint32_t codeMax = compute_locational_code<3>(max, root_size, levels);

        //getting the common code for an node that contains both
        uint32_t code = common_locational_code(codeMin, codeMax);

        //returing the code
        return code;

    }

/**
* @brief	Computes an aabb based on a locational code and the root size
* @param	uint32_t locational_code
* @param	uint32_t root_size
* @return	    aabb    
**/
    aabb     compute_bv(uint32_t locational_code, uint32_t root_size)
    {
        //the aabb to return
        aabb bv;

        //getting the depth of the code
        uint32_t depth = locational_code_depth(locational_code);

        //getting the half of the size of the root
        uint32_t scale = root_size >> 1;

        //initializing the min and max values
        bv.mMin = glm::vec3(-static_cast<int>(scale), -static_cast<int>(scale), -static_cast<int>(scale));
        bv.mMax = glm::vec3(scale, scale, scale);

        //getting on which bit start to check
        uint32_t levelStart = glm::log2(locational_code) - 1;

        //the current level
        int currLevel = 0;

        //for each level of depth on the code
        for (uint32_t i = 0; i < depth; i++)
        {
            //for xyz
            for (int j = 0; j < 3; j++)
            {
                //getting the bit position to check
                int bitPos = levelStart - (currLevel * 3 + (2 - j));

                //if is 1 move the minimum point
                if (locational_code & (1 << bitPos))
                    bv.mMin[j] += scale;
                else//if is a 0 move the maximum point
                    bv.mMax[j] -= scale;
            }

            //right shifting the scale
            scale >>= 1;

            //increasing the level in which we are
            currLevel++;
        }

        //returning the bounding volume
        return bv;
    }

/**
* @brief	Computes the depth of a given locational code
* @param	uint32_t lc
* @return	    uint32_t
**/
    uint32_t locational_code_depth(uint32_t lc)
    {
        //if the locational code is 1 then is refering to the root, so the level is 0
        if (lc == 1)
            return 0;

        //variable to store the depth of the code
        uint32_t depth = 0;

        //while is bigger than 1
        while (lc > 1)
        {
            //right shift the locational code 3 bits
            lc >>= 3;

            //increase the depth counter
            depth++;
        }

        //returing the depth
        return depth;
    }

/**
* @brief	Gets the common locational code of two given ones
* @param	uint32_t lc1
* @param	uint32_t lc2
* @return	    uint32_t
**/
    uint32_t common_locational_code(uint32_t lc1, uint32_t lc2)
    {
        //if any of the two codes is the root return 1
        if (lc1 == 1)
            return lc1;
        
        if (lc2 == 1)
            return lc2;

        //if the codes are the same return the first one
        if (lc1 == lc2)
            return lc1;

        //the starting locational code
        uint32_t common = 1;

        //getting the staring bits for each code
        int mostBit1 = glm::log2(lc1) - 1;
        int mostBit2 = glm::log2(lc2) - 1;

        //boolean to loop
        bool loop = true;

        //loop to get the common code
        while (loop)
        {
            //if the  bit to check is negative return
            if (mostBit1 < 2 || mostBit2 < 2)
                break;

            //checking if both bits representing x y and z coordinates match
            bool bothZ0 = ((lc1 & (1 << mostBit1)) == 0) && ((lc2 & (1 << mostBit2)) == 0);
            bool bothZ1 = (lc1  & (1 << mostBit1))       && (lc2  & (1 << mostBit2));

            bool bothY0 = ((lc1 & (1 << (mostBit1 - 1))) == 0) && ((lc2 & (1 << (mostBit2 - 1))) == 0);
            bool bothY1 = (lc1  & (1 << (mostBit1 - 1)))       && (lc2  & (1 << (mostBit2 - 1)));

            bool bothX0 = ((lc1 & (1 << (mostBit1 - 2))) == 0) && ((lc2 & (1 << (mostBit2 - 2))) == 0);
            bool bothX1 = (lc1  & (1 << (mostBit1 - 2)))       && (lc2  & (1 << (mostBit2 - 2)));

            //if codes match
            bool codeMatch = (bothZ0 || bothZ1) && (bothY0 || bothY1) && (bothX0 || bothX1);
            if (codeMatch)
            {
                //left shift and insert the values
                common <<= 3;

                if (bothZ1)
                    common = common | (1 << 2);

                if (bothY1)
                    common = common | (1 << 1);

                if (bothX1)
                    common = common | (1 << 0);

            }
            else//if do not match end the loop
                loop = false;

            //decreasing 3 the counts
            mostBit1 -= 3;
            mostBit2 -= 3;
        }

        //returing the code;
        return common;
    }

}