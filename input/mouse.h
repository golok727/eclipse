#pragma once
#include <array>
#include <string>
namespace eclipse::input{
  class mouse{
    public:
      mouse(){}
      ~mouse(){}
      static void Inititlaize();
      static void Update();


      inline static int X(){return x;}
      inline static int Y(){return y;}

      inline static int DX(){return x - xLast;}
      inline static int DY(){return y - yLast;}
      static bool Buttons(int button);
      static bool ButtonsDown(int button);
      static bool ButtonsUp(int button);
      
    private:
      constexpr static const int ButtonCount = 5;
      static std::array<bool, ButtonCount>buttons;       
      static std::array<bool, ButtonCount>buttonsLast;       
      static int x , xLast;
      static int y, yLast;
  };
  enum{
    ECLIPSE_INPUT_MOUSE_FIRST = 1,
    ECLIPSE_INPUT_MOUSE_LEFT = ECLIPSE_INPUT_MOUSE_FIRST,
    ECLIPSE_INPUT_MOUSE_MIDDLE = 2,
    ECLIPSE_INPUT_MOUSE_RIGHT = 3,
    ECLIPSE_INPUT_MOUSE_X1 = 4,
    ECLIPSE_INPUT_MOUSE_X2 = 5,
    ECLIPSE_INPUT_MOUSE_LAST= 5,
  };
}
