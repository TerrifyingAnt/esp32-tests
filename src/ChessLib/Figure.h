class Figure {
    private:
        int x, y; // координаты фигуры на доске
    
    public: 
        Figure(); // конструктор
        virtual void getAvailableMoves(int[][] field);
}