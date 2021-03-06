/*---------------------------------------------------------------------
  Adaptació del còdic de Tutorial Days 3: "Bresenham Lines" 



  ---------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>



void  DrawLine(int x1, int y1, int x2, int y2, unsigned short color) {
  int yStep = SCREEN_WIDTH;
  int xStep = 1;
  int xDiff = x2 - x1;
  int yDiff = y2 - y1;

  int errorTerm = 0;
  int offset = y1 * SCREEN_WIDTH + x1;
  int i;

  //need to adjust if y1 > y2
  if (yDiff < 0)    {
      yDiff = -yDiff;       //absolute value
      yStep = -yStep;       //step up instead of down
    }

  //same for x
  if (xDiff < 0)    {
      xDiff = -xDiff;
      xStep = -xStep;
    }
  
  //case for changes more in X than in Y
  if (xDiff > yDiff)   {
      for (i = 0; i < xDiff + 1; i++)	{
	  VRAM_A[offset] = color;
	  offset += xStep;
	  errorTerm += yDiff;
	  if (errorTerm > xDiff)	    {
	      errorTerm -= xDiff;
	      offset += yStep;
	    }
	}
    }//end if xdiff > ydiff
  //case for changes more in Y than in X
  else    {
      for (i = 0; i < yDiff + 1; i++)	{
	  VRAM_A[offset] = color;
	  offset += yStep;
	  errorTerm += xDiff;
	  if (errorTerm > yDiff) 	    {
	      errorTerm -= yDiff;
	      offset += xStep;
	    }
	}
    }

} // DrawLine


int main( void ) {

  touchPosition touch;

  int oldX = 0;
  int oldY = 0;

  videoSetMode(MODE_FB0);
  vramSetBankA(VRAM_A_LCD);

  lcdMainOnBottom();

  while(1)     {
    scanKeys();
    touchRead(&touch);
    if(!(keysDown() & KEY_TOUCH) && (keysHeld() & KEY_TOUCH)) {
      DrawLine(oldX, oldY, touch.px, touch.py, rand());
    }
    oldX = touch.px;
    oldY = touch.py;
    swiWaitForVBlank();
  }
  return 0;
} // Main

