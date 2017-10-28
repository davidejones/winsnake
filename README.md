# winsnake

![anim](https://raw.githubusercontent.com/davidejones/winsnake/master/anim.gif)

## A win32 api based snake game
The game is fixed to a 800x600 non resizable window. It has the keyboard controls set to the arrow keys.

I split the 800x600 by 10x10 pixel size blocks and stored that in a 2d vector. This 2d vector represents the board state which i update and manipulate and then i write that to the bitmap memory that is being blit to the screen.
Its not the most efficient way of doing things but it works as a quick demo game.

### Thanks
Thank you [handmadehero](https://handmadehero.org/) for the windowing/win32 api setup