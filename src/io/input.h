#ifndef __INPUT_H__
#define __INPUT_H__




#include "main.h"




#define KEY_AMOUNT 9


typedef enum _KEY
{
    KEY_ESC,
    KEY_ENTER,
    KEY_SPACE,
    KEY_LSHIFT,
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F
} KEY;




void inputUpdate();


u8 isKeyPressed(KEY key);


u8 isKeyDown(KEY key);




#endif // !__INPUT_H__
