#include <nds.h>
#include <stdio.h>

//Referencias graficas que se generan en "build" por el Makefile "automágicamente"
#include <gfx_ball.h>
#include <gfx_brick.h>
#include <gfx_gradient.h>
#include "ball.h"


//#define NPILOTETES 51
#define PROTA       0
// de 1 a 50, inclosos són NPCs, avatars contrincants!
ball g_ball;
void resetBall( void );

void setupGraphics( int *bg0, int *bg1 );
//void updateGraphics( int bg0, int bg1 );
void updateGraphics( int bg0, int bg1 );
void processLogic( void );


int main( void )
{
    int bg0, bg1;
 
    setupGraphics( &bg0, &bg1 );
    resetBall();
    
    while(1)
    {
        //Periodo de Renderizado
	//Actualizacion objetos del juego (Moverlos alrededor,                        
        //calcular velocidades, etc) 
	//update_logic --> updateLogic(); --> processLogic
	processLogic();
        //printf("Escriu alguna cosa"); NO ESCRIU RES!

	//Espera para periodo vblank
	swiWaitForVBlank();
		
	//Periodo vblank: (seguro modificar graficos)
	//mover graficos alrededor
	updateGraphics( bg0, bg1 );
    }
}


#define ALTURA_BOLA (c_diam << 8)

//-----------------------------------------------------------
// reset ball attributes
//-----------------------------------------------------------
void resetBall( void )
{
  // use sprite index 0 (0->127)
  g_ball.sprite_index = 0;

  // use affine matrix 0 (0->31)
  g_ball.sprite_affine_index = 0;

  // X = 128.0
  g_ball.x = 128 << 8;
  
  // Y = 64.0
  g_ball.y = 64 << 8;
  
  // start X velocity a bit to the right
  g_ball.xvel = 100 << 4;
  
  // reset Y velocity
  g_ball.yvel = 0;
  
  g_ball.height = ALTURA_BOLA;
}


// Teselas (tiles) para los fondos
#define tile_empty		0 //tile 0 = empty
#define tile_brick		1 //tile 1 = brick
#define tile_gradient           2 //tile 2 = gradient

//Paletas de las teselas de los fondos
#define pal_bricks              0 //paleta brick (entrada 0->15)
#define pal_gradient            1 //paleta gradient (entrada 16->31)

#define backdrop_colour	        RGB8( 190, 255, 255 )

// Sprite
#define tiles_ball              0 // ball tiles (16x16 tile 0->3)
#define sprites                 ((SpriteEntry*)OAM)
#define pal_ball                0 // ball palette (entry 0->15)
//#define pal_ballRed             1 // ball palette (entry 0->15)

/* Ja no fa falta:: En sprite.h:: SpriteEntry -->  attribute[3];
typedef struct t_spriteEntry
{
 u16 attr0;
 u16 attr1;
 u16 attr2;
 u16 affine_data;
} spriteEntry;
*/


void setupGraphics( int *bg0, int *bg1 )
{
 int n, x, y; 
 // Fondos, niveles o capas de la escena
 u16 *bg0map, *bg1map;

 // put the main screen on the bottom lcd
 lcdMainOnTop(); // ...Bottom();
        
 // 1.1
 vramSetBankE( VRAM_E_MAIN_BG );
 vramSetBankF( VRAM_F_MAIN_SPRITE );
      
  
 *bg0 = bgInit(0, BgType_Text4bpp, BgSize_T_256x256, 1, 0); 
 *bg1 = bgInit(1, BgType_Text4bpp, BgSize_T_256x256, 2, 1); 

 // 1.2: generar el primer banco de tile por borrado a cero
 for( n = 0; n < 16; n++ )     {       BG_GFX[n] = 0;    }
 
 //Cargando los graficos para las teselas de las capas
 dmaCopy( gfx_brickTiles, (BG_GFX + (tile_brick * 16)), gfx_brickTilesLen );      
 dmaCopy( gfx_gradientTiles, (BG_GFX + (tile_gradient * 16)),            gfx_gradientTilesLen ); 
 
 //Cargando las paletas para las teselas de las capas
 dmaCopy( gfx_brickPal, BG_PALETTE, gfx_brickPalLen );
 //dmaCopy( gfx_gradientPal, BG_PALETTE+(pal_gradient*16), gfx_gradientPalLen );
 dmaCopy( gfx_gradientPal+1, BG_PALETTE+(pal_gradient*16)+1, gfx_gradientPalLen );
    
 //Asignar Color de fondo
 BG_PALETTE[0] = backdrop_colour;
 BG_PALETTE[1] = RGB8( 0, 255, 0 );
 BG_PALETTE[2] = RGB8( 255, 0, 0 );
 
 bg0map = bgGetMapPtr( *bg0 );
 bg1map = bgGetMapPtr( *bg1 );

 //Construir los mapas
 for( n = 0; n < 1024; n++ ) bg0map[n] = 0;
  
 for( x = 0; x < 32; x++ )    {
     for( y = 18; y < 24; y++ ) 
     {
         // magical formula to calculate if the tile needs to be flipped.
         int hflip = (x & 1) ^ (y & 1);
            
         // set the tilemap entry
         bg0map[x + y * 32] = tile_brick | (hflip << 10) | (pal_bricks << 12);
     }
 }
    
 for( n = 0; n < 1024; n++ ) bg1map[n] = 0;
 for( x = 0; x < 32; x++ ) {
     for( y = 0; y < 8; y++ )
     {
           int tile = tile_gradient + y;
           bg1map[ x + y * 32 ] = tile | (pal_gradient << 12);
     }
 }
    
 // Combinar el gradiente (BG1) con el "backdrop".
 REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BACKDROP;
 //De l'original es veu molt brillant, aixina que el baixa 
 //REG_BLDALPHA = (16<<8) + (4); //SRC_BG1*16 + BACKDROP*4
 REG_BLDALPHA = (16<<8) + (16); //SRC_BG1*16 + BACKDROP*4
  //
  // void bgClearControlBits( 	int  	id, 	u16  	bits 	) ; ?????	
  //

 // Inicializar el modo de vídeo
 videoSetMode( MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE );
 // Inicializar el uso de sprites
 oamInit(&oamMain, SpriteMapping_1D_32, false); //!extPalette

 // Cargar los sprites
 dmaCopy( gfx_ballTiles, SPRITE_GFX, gfx_ballTilesLen );
 dmaCopy( gfx_ballPal, SPRITE_PALETTE, gfx_ballPalLen );
 //dmaCopy( gfx_ballPal, SPRITE_PALETTE+(pal_ballRed*16), gfx_ballPalLen );
} // Fi de setupGraphics



int g_camera_x;
int g_camera_y;

//-----------------------------------------------------------
// update graphical information (call during vblank)
//-----------------------------------------------------------
// Menejar pilota i càmera
void updateGraphics( int bg0, int bg1 )
{
 // Ací deuria anar un 
 //oamUpdate( &oamMain );
  ballRender( &g_ball, 0, 0 );    

  REG_BG0HOFS = g_camera_x >> 8;
}
void updateGraphics_00( int bg0, int bg1 )
//-----------------------------------------------------------
{
 // Ací deuria anar un 
 //oamUpdate( &oamMain );
   
    // attribute0: set vertical position 0->screen_height-sprite_height,
    // other default options will be okay (default == zeroed)
    //sprites[n].attr0 = rand() % (192 - 16);
//    sprites[n].attribute[0] = rand() % (192 - 16);
    //No! sprites[n].y = rand() % (192 - 16);   
    
    // attribute1: set horizontal position 0->screen_width-sprite_width
    // also set 16x16 size mode
    //sprites[n].attr1 = (rand() % (256 - 16)) + ATTR1_SIZE_16;
//    sprites[n].attribute[1] = (rand() % (256 - 16)) + ATTR1_SIZE_16;
    // No! sprites[n].x = (rand() % (256 - 16)) + ATTR1_SIZE_16;
 //
 //oamSetXY( &oamMain, n, 
 //          (rand() % (256 - 16)) + ATTR1_SIZE_16, 
 //          rand() % (192 - 16) );
 //
  
 // attribute0: select tile number and palette number
 //sprites[n].attr2 = tiles_ball + (pal_ball << 12);
 //sprites[n].attr2 = tiles_ball + (pal_ball << 12);
 //sprites[n].attribute[2] = tiles_ball + (pal_ball << 12);
   
 //sprites[n].gfxIndex = tiles_ball;
 //oamSetGfx( &oamMain, n, SpriteSize_8x8, SpriteColorFormat_16Color, tiles_ball + (pal_ball << 12));
 //oamSetGfx( &oamMain, n, SpriteSize_16x8, SpriteColorFormat_16Color, tiles_ball);
 	
 //if (n == 0)
 //    sprites[n].palette = (pal_ball << 12);
 //    oamSetPalette( &oamMain, n, (pal_ball << 12) );
 //else
 //    //sprites[n].palette = (pal_ballRed << 12);
 //   //oamSetPalette( &oamMain, n, (pal_ballRed << 12) );
 //   oamSetPalette( &oamMain, n, (pal_ballRed) );
 
 
  //REG_BG0HOFS = g_camera_x >> 8;
 //bgScroll( bg0, g_camera_x, 0 );
 bgSetScroll( bg0, g_camera_x, 0 );
 bgUpdate(); // Must be called once per frame to update scroll/scale/and rotation of backgrounds. 
 
}

#define x_tweak (1<<2) // for user input  (Ho baixe de (2<<8)
#define y_tweak 50  // for user input: quan més gran més salta

void processInput( void ) {

 scanKeys();

 int keysh = keysHeld();
 // process user input
 if( keysh & KEY_UP ) { // check  if UP is pressed --> tweak y velocity of ball
  g_ball.yvel -= y_tweak;
 }
 
 if( keysh & KEY_DOWN ){ // check if DOWN is pressed --> tweak y velocity of ball
  g_ball.yvel += y_tweak;
 }

 if( keysh & KEY_LEFT ) {// check if LEFT is pressed --> tweak x velocity
  g_ball.xvel -= x_tweak;
 }

 if( keysh & KEY_RIGHT ) { // check if RIGHT is pressed --> tweak y velocity
  g_ball.xvel += x_tweak;
 }
 
} // Fi de processInput


 void updateCamera( void ) {
 // cx = desired camera X (en format 24.8)
 int cx = ((g_ball.x)) - (128 << 8); 

 // dx = difference between desired and current position
 int dx;
 dx = cx - g_camera_x;

 // 10 is the minimum threshold
 if( dx > 10 || dx < -10 )  //10
   dx = (dx * 50) >> 1; // scale the value by some amount // 50 i 10

 // add the value to the camera X position
 g_camera_x += dx;

 // camera Y is always 0
 g_camera_y = 0;
 
} // Fi de updateCamera

 
void processLogic( void )
{
 processInput();
 ballUpdate( &g_ball );

 /*
 "Whoops, the ball went out of the screen."
To fix this, we will add a 'camera' that will follow the ball around the screen. Make two more global variables.
 */
 updateCamera();
} // fi de processLogic

 


