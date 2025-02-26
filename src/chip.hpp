class Chip8 {

    public:
        Chip8();
        void LoadROM(char const* filename);


    private:
        std::default_random_engine randGen;
	    std::uniform_int_distribution<uint8_t> randByte;




};
    


