#pragma once

namespace cs350 {
    class texture
    {
      private:
        std::vector<unsigned> m_pixels{};
        unsigned              m_width{};
        unsigned              m_height{};

      public:
        texture();
        ~texture();

        /**
         *
         */
        void destroy();

        /**
         * Deletes previous texture and creates a new one empty
         * @param width 
         * @param height 
         */
        void resize(unsigned width, unsigned height);

        /**
         * Sets a particular pixel. [RGBA] 0xrrggbbaa format, ie ( 0x705030ff )
         * @param x 
         * @param y 
         * @param rgba 
         */
        void set_pixel(unsigned x, unsigned y, unsigned rgba);

        /**
         * Gets the pixel at given coordinate
         * @param x 
         * @param y 
         * @return unsigned [RGBA] format
         */
        unsigned  get_pixel(unsigned x, unsigned y) const;
        unsigned& get_pixel(unsigned x, unsigned y);

        /**
         * Saves the texture in a png
         * @param path 
         */
        void save(char const* path) const;

        /**
         * Loads a texture from a png
         * @param path
         */
        void load(char const* path);

        unsigned width() const noexcept { return m_width; }
        unsigned height() const noexcept { return m_height; }
    };
}