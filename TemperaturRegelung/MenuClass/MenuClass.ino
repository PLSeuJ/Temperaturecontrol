class Menu {
  private:
    byte _state;
  public:
    Menu() {
      init();
    }
    void init() {
      build();
    }
    void build() {
      
    }
    //void update() {
    //
    //}
    byte getState() {
      return _state;
    }
};
