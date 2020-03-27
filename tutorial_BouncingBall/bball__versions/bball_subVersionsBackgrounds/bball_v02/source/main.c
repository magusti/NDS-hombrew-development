
#include <nds.h>
#include <stdio.h>

//Referenias Graficas que se generan en "build" por el Makefile "automágicamente"
#include "gfx_brick.h"
#include "gfx_gradient.h"

void setupGraphics();
void update_logic();	
void update_graphics();

int main( void )
{
	//irqEnable( IRQ_VBLANK );	//Habilitar interrupcion vblank
	setupGraphics();
	while(1)
	{
		//Periodo de Renderizado
		//Actualizacion objetos del juego (Moverlos alrededor,                        
                       //calcular velocidades, etc) 
		update_logic();
		
		//Espera para periodo vblank
		swiWaitForVBlank();
		
		//Periodo vblank: (seguro modificar graficos)
		//mover graficos alrededor
		update_graphics();
	}
}



//Entradas tile
// 1
#define tile_empty		0 //tile 0 = empty
#define tile_brick		1 //tile 1 = brick
#define tile_gradient	      2 //tile 2 = gradient

//macro para calcular memoria BG VRAM
//direccion con el indice de tile
#define tile2bgram(t) (BG_GFX + (t) *16)

//Paletas de entrada
#define pal_bricks	0 //paleta brick (entrada 0->15)
#define pal_gradient	1 //paleta gradient (entrada 16->31)

#define backdrop_colour	RGB8( 190, 255, 255 )
#define pal2bgram(p)	(BG_PALETTE + (p) * 16)


// 2
#define bg0map    ((u16*)BG_MAP_RAM(1))
#define bg1map    ((u16*)BG_MAP_RAM(2))


void setupGraphics( void )
{
    // 1.1
       vramSetBankE( VRAM_E_MAIN_BG );
       vramSetBankF( VRAM_F_MAIN_SPRITE );
      
// 1.2
    //generar el primer banco de tile por borrado a cero
    int n;
    for( n = 0; n < 16; n++ )
    {
       BG_GFX[n] = 0;
    }
   //Copiando graficos
    dmaCopyHalfWords( 3, gfx_brickTiles, tile2bgram( tile_brick ), gfx_brickTilesLen );
    dmaCopyHalfWords( 3, gfx_gradientTiles, tile2bgram( tile_gradient ), gfx_gradientTilesLen );
    
    // 1.3
    //Paleta direccionada a la memoria de paleta
dmaCopyHalfWords( 3, gfx_brickPal, pal2bgram( pal_bricks ), gfx_brickPalLen );
dmaCopyHalfWords( 3, gfx_gradientPal, pal2bgram( pal_gradient ), gfx_gradientPalLen );
	
//Asignar Color de fondo
BG_PALETTE[0] = backdrop_colour;

    // 1.4
// Actualitzat per 2.3
// videoSetMode( MODE_0_2D );
 

// 2.1

// libnds prefixes the register names with REG_BGxCNT
REG_BG0CNT = BG_MAP_BASE(1);
REG_BG1CNT = BG_MAP_BASE(2);
//int n;
for( n = 0; n < 1024; n++ ) bg0map[n] = 0;

// 2.2
int x, y;
for( x = 0; x < 32; x++ )
{
    for( y = 0; y < 6; y++ ) 
    {
         // magical formula to calculate if the tile needs to be flipped.
         int hflip = (x & 1) ^ (y & 1);
            
         // set the tilemap entry
         bg0map[x + y * 32] = tile_brick | (hflip << 10) | (pal_bricks << 12);
    }
}
// 2.3
videoSetMode( MODE_0_2D | DISPLAY_BG0_ACTIVE );

// 2.4  // Una volta ja pinte el sol baix, no fa falta acò
REG_BG0VOFS = 112;

// 3.1
for( n = 0; n < 1024; n++ ) bg1map[n] = 0;

// 3.2
for( x = 0; x < 32; x++ )
{
     for( y = 0; y < 8; y++ )
     {
           int tile = tile_gradient + y;
           bg1map[ x + y * 32 ] = tile | (pal_gradient << 12);
     }
}
videoSetMode( MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE );

// Combinar el gradiente (BG1) con el "backdrop".
REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BACKDROP;
//REG_BLDALPHA = (16) + (16<<8); De l'original es veu molt brillant, aixina que el baixa 
REG_BLDALPHA = (4) + (16<<8);

} // Fi de setupGraphics


void update_logic()
{
    
}

void update_graphics()
{
 

}