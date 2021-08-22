
#include "ui.h"
#include "u8g2.h"
#include "ui_u8g2.h"


/*

uint8_t uif_template(ui_t *ui, uint8_t msg)
{
  //ui->dflags                          UIF_DFLAG_IS_CURSOR_FOCUS       UIF_DFLAG_IS_TOUCH_FOCUS
  //uif_get_cflags(ui->uif)       UIF_CFLAG_IS_CURSOR_SELECTABLE
  //uif_get_data(ui->uif)
  switch(msg)
  {
    case UIF_MSG_DRAW:
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;
  }
  return 0;
}
*/

/*

  Description: 
    Draws normal or inverted text with optional frame around text.
    The text (and the frame) can be horizontally centered around the provided reference position.
    This procedure will use the current draw color and current font. The height of the frame
    depends on the setting of setFontRefHeightText(), setFontRefHeightExtendedText() or setFontRefHeightAll()


  Note 1: drawColor 2 (XOR) is not supported. Result will be broken with draw color 2.
  Note 2: This procedure will enforce font mode 1 (transparent mode)
  Note 3: Some fonts may add an extra gap on the right side. This is a font problem and can not be avoided (for example inb16 has this problem).
  

  The height of the button is defined by the current font and
    setFontRefHeightText,
    setFontRefHeightExtendedText
    setFontRefHeightAll
  Right padding might be influenced by the font.
  For example u8g2_font_inb16 may add an extra pixel gap (increase padding by one) 
  on the right side.
  The current draw color is considered, but only draw color 0 and 1 are supported
  
  This function requires transparent font mode: setFontMode(1)
  
  Side effect: Font transparent mode is enabled setFontMode(1)



  if padding_h is larger than or equal to half of the display width, then the button size is maximized to the display size
*/



/*

  flags:

    U8G2_BTN_BW1 0x01
    U8G2_BTN_BW2 0x02
    U8G2_BTN_BW3 0x03


    U8G2_BTN_SHADOW0 0x08
    U8G2_BTN_SHADOW1 0x10
    U8G2_BTN_SHADOW2 0x18

    U8G2_BTN_INV 0x20

    U8G2_BTN_HCENTER 0x40


  Total size without shadow: width+2*padding_h+2*border
  Total size with U8G2_BTN_SHADOW0: width+2*padding_h+3*border
  Total size with U8G2_BTN_SHADOW1: width+2*padding_h+3*border+1
  Total size with U8G2_BTN_SHADOW2: width+2*padding_h+3*border+2
*/

void u8g2_DrawButtonFrame(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t flags, u8g2_uint_t text_width, u8g2_uint_t padding_h, u8g2_uint_t padding_v)
{
  u8g2_uint_t w = text_width;
  
  u8g2_uint_t xx, yy, ww, hh;
  
  u8g2_uint_t border_width = flags & U8G2_BTN_BW_MASK;

  int8_t a = u8g2_GetAscent(u8g2);
  int8_t d = u8g2_GetDescent(u8g2);
  
  for(;;)
  {

    xx = x;
    xx -= padding_h;
    xx -= border_width;
    ww = w+2*padding_h+2*border_width;
    
    yy = y;
    yy += u8g2->font_calc_vref(u8g2);
    yy -= a;
    yy -= padding_v;
    yy -= border_width;
    hh = a-d+2*padding_v+2*border_width;
    if ( border_width == 0 )
      break;
    u8g2_DrawFrame(u8g2, xx, yy, ww, hh);
    
    if ( flags & U8G2_BTN_SHADOW_MASK )
    {
      if ( border_width == (flags & U8G2_BTN_BW_MASK) )
      {
        u8g2_uint_t i;
        u8g2_uint_t shadow_gap = (flags & U8G2_BTN_SHADOW_MASK) >> U8G2_BTN_SHADOW_POS;
        shadow_gap--;
        for( i = 0; i < border_width; i++ )
        {
          u8g2_DrawHLine(u8g2, xx+border_width+shadow_gap,yy+hh+i+shadow_gap,ww);
          u8g2_DrawVLine(u8g2, xx+ww+i+shadow_gap,yy+border_width+shadow_gap,hh);
        }
      }
    }
    border_width--;
  }
  if ( flags & U8G2_BTN_INV )
  {
    uint8_t color_backup = u8g2->draw_color;
    u8g2_SetDrawColor(u8g2, 2);         /* XOR */
    u8g2_DrawBox(u8g2, xx, yy, ww, hh);
    u8g2_SetDrawColor(u8g2, color_backup);
  }
}

void u8g2_DrawButtonUTF8(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t flags, u8g2_uint_t width, u8g2_uint_t padding_h, u8g2_uint_t padding_v, const char *text)
{
  u8g2_uint_t w = u8g2_GetUTF8Width(u8g2, text);
  
  //u8g2_uint_t xx, yy, ww, hh;
  
  u8g2_uint_t text_x_offset = 0;        // used for U8G2_BTN_PADWIDTH mode

  //int8_t a = u8g2_GetAscent(u8g2);
  //int8_t d = u8g2_GetDescent(u8g2);
  //uint8_t color_backup = u8g2->draw_color;


  if ( flags & U8G2_BTN_HCENTER )
    x -= (w+1)/2;

  if ( w < width )
  {
    if ( flags & U8G2_BTN_HCENTER )
    {
      text_x_offset = (width-w)/2;
    }
    w = width;
  }
  
  u8g2_SetFontMode(u8g2, 1);    
  u8g2_DrawUTF8(u8g2, x,y, text);
  u8g2_DrawButtonFrame(u8g2, x-text_x_offset, y, flags, w, padding_h, padding_v);
  
}




/*
  Shadow is not supported
  Note: radius must be at least as high as the border width

  border width | good radius values
  1             | 3, 5, 7, 8, ...
  2             | 3, 5, 7, 8, ...
  
*/
void u8g2_DrawRButtonUTF8(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t flags, u8g2_uint_t padding_h_or_width, u8g2_uint_t padding_v, u8g2_uint_t r, const char *text)
{
  u8g2_uint_t w = u8g2_GetUTF8Width(u8g2, text);
  //u8g2_uint_t w = u8g2_GetExactStrWidth(u8g2, text);
  
  u8g2_uint_t xx, yy, ww, hh;
  
  u8g2_uint_t border_width = flags & U8G2_BTN_BW_MASK;
  u8g2_uint_t padding_h = padding_h_or_width;
  u8g2_uint_t text_x_offset = 0;        // used for U8G2_BTN_PADWIDTH mode

  int8_t a = u8g2_GetAscent(u8g2);
  int8_t d = u8g2_GetDescent(u8g2);
  uint8_t color_backup = u8g2->draw_color;


  if ( flags & U8G2_BTN_HCENTER )
    x -= w/2;

  if ( flags & U8G2_BTN_PADWIDTH )
  {
    padding_h = 0;
    if ( w < padding_h_or_width )
    {
      if ( flags & U8G2_BTN_HCENTER )
      {
        text_x_offset = (padding_h_or_width-w)/2;
      }
      w = padding_h_or_width;
    }
  }
  
  
  u8g2_SetFontMode(u8g2, 1);
    
  for(;;)
  {
    if ( padding_h >= u8g2_GetDisplayWidth(u8g2)/2 )    // padding_h is zero if U8G2_BTN_PADWIDTH is set
    {
      xx = (flags & U8G2_BTN_BW_MASK) - border_width;
      ww = u8g2_GetDisplayWidth(u8g2);
      ww -= 2*((flags & U8G2_BTN_BW_MASK) - border_width);
      //printf("xx=%d ww=%d\n", xx, ww);
      //printf("clip_x1=%d clip_x0=%d\n", u8g2->clip_x1, u8g2->clip_x0);
    }
    else
    {
      xx = x;
      xx -= text_x_offset;
      xx -= padding_h;
      xx -= border_width;
      ww = w+2*padding_h+2*border_width;
    }
    
    yy = y;
    yy += u8g2->font_calc_vref(u8g2);
    yy -= a;
    yy -= padding_v;
    yy -= border_width;
    hh = a-d+2*padding_v+2*border_width;
    if ( border_width == 0 )
      break;
    u8g2_DrawRFrame(u8g2, xx, yy, ww, hh, r);
    if ( (flags & U8G2_BTN_BW_MASK) > 1 )
      u8g2_DrawRFrame(u8g2, xx, yy, ww, hh, r+1);
    
    border_width--;
    if ( r > 1 )
      r--;
  }
  if ( flags & U8G2_BTN_INV )
  {
    u8g2_DrawRBox(u8g2, xx, yy, ww, hh,r);
    u8g2_SetDrawColor(u8g2, 1-u8g2->draw_color);
  }
  u8g2_DrawUTF8(u8g2, x,y, text);
  u8g2_SetDrawColor(u8g2, color_backup);
}

/*=========================================================================*/
/* helper function */

u8g2_uint_t ui_get_x(ui_t *ui)
{
  return ui->x;
}

u8g2_uint_t ui_get_y(ui_t *ui)
{
  return ui->y;
}

u8g2_t *ui_get_U8g2(ui_t *ui)
{
  return (u8g2_t *)(ui->graphics_data);
}

/*=========================================================================*/
/* incomplete (draw only) field functions */


/*
  xy: yes, arg: no, text: yes
*/
uint8_t uif_frame_button_bold_select_u8g2(ui_t *ui, uint8_t msg)
{
  u8g2_t *u8g2 = ui_get_U8g2(ui);
  u8g2_uint_t flags = U8G2_BTN_HCENTER | U8G2_BTN_PADWIDTH;
  switch(msg)
  {
    case UIF_MSG_DRAW:
      //printf("DRAW fds=%p uif=%p text=%s\n", ui->fds, ui->uif, ui->text);
      //u8g2_DrawStr(ui_get_U8g2(ui), ui_get_x(ui), ui_get_y(ui), ui->text);
      //u8g2_DrawButtonUTF8(ui_get_U8g2(ui), ui_get_x(ui), ui_get_y(ui), U8G2_BTN_HCENTER | U8G2_BTN_BW1 | U8G2_BTN_INV, 4, 1, ui->text);
      //u8g2_DrawButtonUTF8(ui_get_U8g2(ui), ui_get_x(ui), ui_get_y(ui), U8G2_BTN_HCENTER | U8G2_BTN_PADWIDTH | U8G2_BTN_SHADOW2 | 2, 100, 1, ui->text);
      //u8g2_DrawRButtonUTF8(ui_get_U8g2(ui), ui_get_x(ui), ui_get_y(ui), U8G2_BTN_HCENTER  | U8G2_BTN_INV | 3, 2, 1, ui->text);
      if ( ui->dflags & UIF_DFLAG_IS_CURSOR_FOCUS )
      {
        flags |= 3;
      }
      else
      {
        flags |= 1;
      }
      u8g2_DrawButtonUTF8(u8g2, ui_get_x(ui), ui_get_y(ui), flags, u8g2_GetDisplayWidth(u8g2)/2 - 10, 0, 1, ui->text);
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;    
  }
  return 0;
}

/*

  uint8_t uif_half_width_frame_button_invers_select_u8g2(ui_t *ui, uint8_t msg)

  Description:
    A button with size equal to display width / 2 - 10 pixel
    The button has a one pixel frame around the text.
    
  Message Handling: DRAW

  Style
    No Selection: Text + Frame
    Cursor Selection: Inverted text + Frame

  User interface field list (uif):
    flags: UIF_CFLAG_IS_CURSOR_SELECTABLE
    data: not used

  Field definition string (fds):
    xy: Left position of the text (required)
    arg: not used
    text: Button label
    
*/

uint8_t uif_half_width_frame_button_invers_select_u8g2(ui_t *ui, uint8_t msg)
{
  u8g2_t *u8g2 = ui_get_U8g2(ui);
  u8g2_uint_t flags = U8G2_BTN_HCENTER | 1;
  switch(msg)
  {
    case UIF_MSG_DRAW:
      if ( ui_IsCursorFocus(ui) )
      {
        flags |= U8G2_BTN_INV;
      }
      u8g2_DrawButtonUTF8(u8g2, ui_get_x(ui), ui_get_y(ui), flags, u8g2_GetDisplayWidth(u8g2)/2 - 10, 0, 1, ui->text);
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;    
  }
  return 0;
}

/*

  uint8_t uif_line_button_invers_select_u8g2(ui_t *ui, uint8_t msg)

  Description:
    A full line button (covers complete width of the display).
    
  Message Handling: DRAW

  Style
    No Selection: Text only
    Cursor Selection: Inverted text

  User interface field list (uif):
    flags: UIF_CFLAG_IS_CURSOR_SELECTABLE
    data: not used

  Field definition string (fds):
    xy: Left position of the text (required)
    arg: not used
    text: Button label
    
*/
uint8_t uif_line_button_invers_select_u8g2(ui_t *ui, uint8_t msg)
{
  u8g2_t *u8g2 = ui_get_U8g2(ui);
  //u8g2_uint_t flags = U8G2_BTN_HCENTER ;
  u8g2_uint_t flags = 0;
  switch(msg)
  {
    case UIF_MSG_DRAW:
      if ( ui_IsCursorFocus(ui) )
      {
        flags |= U8G2_BTN_INV;
      }
      u8g2_DrawButtonUTF8(u8g2, ui_get_x(ui), ui_get_y(ui), flags, u8g2_GetDisplayWidth(u8g2)-ui_get_x(ui)*2, ui_get_x(ui) , 0, ui->text);
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;    
  }
  return 0;
}


/*=========================================================================*/
/* ready to use field functions */

/*
  xy: yes, arg: no, text: yes
*/

uint8_t uif_label_u8g2(ui_t *ui, uint8_t msg)
{
  switch(msg)
  {
    case UIF_MSG_DRAW:
      u8g2_DrawStr(ui_get_U8g2(ui), ui_get_x(ui), ui_get_y(ui), ui->text);
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;    
  }
  return 0;
}

/*

  uint8_t uif_goto_half_width_frame_button_invers_select_u8g2(ui_t *ui, uint8_t msg)

  Description:
    A button with size equal to display width / 2 - 10 pixel
    The button has a one pixel frame around the text.
    If the selected, then the form will change to the specified form number.
    
  Message Handling: DRAW, CURSOR_SELECT

  Style
    No Selection: Text + Frame
    Cursor Selection: Inverted text + Frame

  User interface field list (uif):
    flags: UIF_CFLAG_IS_CURSOR_SELECTABLE
    data: not used

  Field definition string (fds):
    xy: Left position of the text (required)
    arg: not used
    text: Button label
    
*/
uint8_t uif_goto_half_width_frame_button_invers_select_u8g2(ui_t *ui, uint8_t msg)
{
  switch(msg)
  {
    case UIF_MSG_CURSOR_SELECT:
      return ui_GotoForm(ui, ui->arg, 0);
  }
  return uif_half_width_frame_button_invers_select_u8g2(ui, msg);
}


/*

  uint8_t uif_goto_line_button_invers_select_u8g2(ui_t *ui, uint8_t msg)

  Description:
    A full line button (covers complete width of the display).
    If the selected, then the form will change to the specified form number.
    
  Message Handling: DRAW, CURSOR_SELECT

  Style
    No Selection: Text only
    Cursor Selection: Inverted text

  User interface field list (uif):
    flags: UIF_CFLAG_IS_CURSOR_SELECTABLE
    data: not used

  Field definition string (fds):
    xy: Left position of the text (required)
    arg: If selected, the form will change to the form specified by this argument
    text: Button label
    
*/

uint8_t uif_goto_line_button_invers_select_u8g2(ui_t *ui, uint8_t msg)
{
  switch(msg)
  {
    case UIF_MSG_CURSOR_SELECT:
      return ui_GotoForm(ui, ui->arg, 0);
  }
  return uif_line_button_invers_select_u8g2(ui, msg);
}


/*

  uint8_t uif_input_uint8_invers_select_u8g2(ui_t *ui, uint8_t msg)

  Description:
    Select a number between 0 and 9.
    Only one number is visible.
    The visible option is automatically the selected number.
    The selected number is stored ad the target data location.

  Message Handling: DRAW, SELECT

  Style
    No Selection: Number only
    Cursor Selection: Inverted number

  User interface field list (uif):
    flags: UIF_CFLAG_IS_CURSOR_SELECTABLE
    data: uint8_t *, pointer to a uint8_t variable, which contains the selected option 

  Field definition string (fds):
    xy: Left position of the text (required)
    arg: not used 
    text: not used
    
*/
uint8_t uif_input_uint8_invers_select_u8g2(ui_t *ui, uint8_t msg)
{
  //ui->dflags                          UIF_DFLAG_IS_CURSOR_FOCUS       UIF_DFLAG_IS_TOUCH_FOCUS
  //uif_get_cflags(ui->uif)       UIF_CFLAG_IS_CURSOR_SELECTABLE
  u8g2_t *u8g2 = ui_get_U8g2(ui);
  u8g2_uint_t flags = 0;
  uint8_t *value = (uint8_t *)uif_get_data(ui->uif);
  char buf[6];
  switch(msg)
  {
    case UIF_MSG_DRAW:
      if ( *value > 9 ) *value = 9;
      buf[0] = (char)(*value + '0');
      buf[1] = '\0';
      if ( ui_IsCursorFocus(ui) )
      {
        flags |= U8G2_BTN_INV;
      }
      
      u8g2_DrawButtonUTF8(u8g2, ui_get_x(ui), ui_get_y(ui), flags, u8g2_GetStrWidth(u8g2, "9"), 1, 1, buf);
      
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      (*value)++;
      if ( *value > 9 ) *value = 0;      
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;
  }
  return 0;
}

/*

  uint8_t uif_single_line_option_invers_select_u8g2(ui_t *ui, uint8_t msg)

  Description:
    Select one of several options. First option has value 0.
    Only one option is visible.
    The visible option is automatically the selected option.

  Message Handling: DRAW, SELECT

  Style
    No Selection: Text only
    Cursor Selection: Inverted text

  User interface field list (uif):
    flags: UIF_CFLAG_IS_CURSOR_SELECTABLE
    data: uint8_t *, pointer to a uint8_t variable, which contains the selected option 

  Field definition string (fds):
    xy: Left position of the text (required)
    arg: total width of the selectable option (optional), 
    text: '|' separated list of options
    
*/
uint8_t uif_single_line_option_invers_select_u8g2(ui_t *ui, uint8_t msg)
{
  //ui->dflags                          UIF_DFLAG_IS_CURSOR_FOCUS       UIF_DFLAG_IS_TOUCH_FOCUS
  //uif_get_cflags(ui->uif)       UIF_CFLAG_IS_CURSOR_SELECTABLE
  u8g2_t *u8g2 = ui_get_U8g2(ui);
  u8g2_uint_t flags = 0;
  //u8g2_uint_t flags = 0;
  uint8_t *value = (uint8_t *)uif_get_data(ui->uif);
  switch(msg)
  {
    case UIF_MSG_DRAW:
      if ( ui_fds_get_nth_token(ui, *value) == 0 )
      {
        *value = 0;
        ui_fds_get_nth_token(ui, *value);
      }      
      if ( ui_IsCursorFocus(ui) )
      {
        flags |= U8G2_BTN_INV;
      }
      u8g2_DrawButtonUTF8(u8g2, ui_get_x(ui), ui_get_y(ui), flags, ui->arg, 1, 1, ui->text);
      
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      (*value)++;
      if ( ui_fds_get_nth_token(ui, *value) == 0 ) 
        *value = 0;      
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;
  }
  return 0;
}
   
void u8g2_DrawCheckbox(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t is_checked)
{
  u8g2_DrawFrame(u8g2, x, y-w, w, w);
  if ( is_checked )
  {
    w-=4;
    u8g2_DrawBox(u8g2, x+2, y-w-2, w, w);
  }
}

void u8g2_Draw4Pixel(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w)
{
  u8g2_DrawPixel(u8g2, x,y-1);
  u8g2_DrawPixel(u8g2, x+w-1,y-1);
  u8g2_DrawPixel(u8g2, x+w-1,y-w);
  u8g2_DrawPixel(u8g2, x,y-w);
}

void u8g2_DrawRadio(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t is_checked)
{
  uint8_t color_backup = u8g2->draw_color;
  u8g2_DrawCheckbox(u8g2, x,y,w,is_checked);
  u8g2_SetDrawColor(u8g2, 2);
  u8g2_Draw4Pixel(u8g2, x,y,w);
  if ( is_checked )
  {
    //u8g2_Draw4Pixel(u8g2, x+2,y-2,w-4);
  }
  
  u8g2_SetDrawColor(u8g2, color_backup );
}
   
uint8_t uif_checkbox_u8g2(ui_t *ui, uint8_t msg)
{
  u8g2_t *u8g2 = ui_get_U8g2(ui);
  u8g2_uint_t flags = 0;
  uint8_t *value = (uint8_t *)uif_get_data(ui->uif);
  switch(msg)
  {
    case UIF_MSG_DRAW:
      if ( *value > 1 ) *value = 1;
      if ( ui_IsCursorFocus(ui) )
      {
        flags |= U8G2_BTN_INV;
      }
      
      {
        u8g2_uint_t w = 0;
        u8g2_uint_t a = u8g2_GetAscent(u8g2);
        if ( *value )
          u8g2_DrawCheckbox(u8g2, ui_get_x(ui), ui_get_y(ui), a, 1);
        else
          u8g2_DrawCheckbox(u8g2, ui_get_x(ui), ui_get_y(ui), a, 0);
        
        if ( ui->text[0] != '\0' )
        {
          w =  u8g2_GetUTF8Width(u8g2, ui->text);
          u8g2_SetFontMode(u8g2, 1);
          a += 2;       /* add gap between the checkbox and the text area */
          u8g2_DrawUTF8(u8g2, ui_get_x(ui)+a, ui_get_y(ui), ui->text);
        }
        
        u8g2_DrawButtonFrame(u8g2, ui_get_x(ui), ui_get_y(ui), flags, w+a, 1, 1);
      }
      break;
    case UIF_MSG_FORM_START:
      break;
    case UIF_MSG_FORM_END:
      break;
    case UIF_MSG_CURSOR_ENTER:
      break;
    case UIF_MSG_CURSOR_SELECT:
      (*value)++;
      if ( *value > 1 ) *value = 0;      
      break;
    case UIF_MSG_CURSOR_LEAVE:
      break;
    case UIF_MSG_TOUCH_DOWN:
      break;
    case UIF_MSG_TOUCH_UP:
      break;
  }
  return 0;
  
}

