#ifndef DISPLAY_FIELD_H
#define DISPLAY_FIELD_H

#include <SFML/Graphics.h>

#include "ui_elements.h"

typedef enum DisplayFieldHorizontalAlignment
{
    displayFieldTextAlignLeft,
    displayFieldTextAlignCenter,
    displayFieldTextAlignRight

} DisplayFieldHorizontalAlignment;

typedef enum DisplayFieldVerticalAlignment
{
    displayFieldTextAlignTop,
    displayFieldTextAlignMiddle,
    displayFieldTextAlignBottom

} DisplayFieldVerticalAlignment;


/* Lifecycle */
DisplayField* displayField_create(void);
DisplayField* displayField_copy(const DisplayField*);
void displayField_destroy(DisplayField*);

/* General manipulation */
void displayField_setSize(DisplayField*, sfVector2f);
sfVector2f displayField_getSize(const DisplayField*);

void displayField_setPosition(DisplayField*, sfVector2f);
sfVector2f displayField_getPosition(const DisplayField*);

void displayField_setOrigin(DisplayField*, sfVector2f);
sfVector2f displayField_getOrigin(const DisplayField*);

/* Fill color manipulation */
void displayField_setFillColor(DisplayField*, sfColor);
sfColor displayField_getFillColor(const DisplayField*);

/* Text manipulation */
void displayField_setTextString(DisplayField*, const char*);
const char* displayField_getTextString(const DisplayField*);

void displayField_setTextFont(DisplayField*, const sfFont*);
const sfFont* displayField_getTextFont(const DisplayField*);

void displayField_setCharacterSize(DisplayField*, unsigned int);
unsigned int displayField_getCharacterSize(const DisplayField*);

void displayField_setLetterSpacing(DisplayField*, float);
float displayField_getLetterSpacing(const DisplayField*);

void displayField_setTextColor(DisplayField*, sfColor);
sfColor displayField_getTextColor(const DisplayField*);

void displayField_setTextPadding(DisplayField*, sfVector2f);
sfVector2f displayField_getTextPadding(const DisplayField*);

void displayField_setTextHorizontalAlignment(DisplayField*, DisplayFieldHorizontalAlignment);
DisplayFieldHorizontalAlignment displayField_getTextHorizontalAlignment(const DisplayField*);

void displayField_setTextVerticalAlignment(DisplayField*, DisplayFieldVerticalAlignment);
DisplayFieldVerticalAlignment displayField_getTextVerticalAlignment(const DisplayField*);

void displayField_setTextAlignment(DisplayField*, DisplayFieldHorizontalAlignment, DisplayFieldVerticalAlignment);

/* Texture manipulation */
void displayField_setTexture(DisplayField*, const sfTexture*, sfBool);
const sfTexture* displayField_getTexture(const DisplayField*);

void displayField_setTextureRect(DisplayField*, sfIntRect);
sfIntRect displayField_getTextureRect(const DisplayField*);

/* Outline manipulation */
void displayField_setOutlineColor(DisplayField*, sfColor);
sfColor displayField_getOutlineColor(const DisplayField*);

void displayField_setOutlineThickness(DisplayField*, float);
float displayField_getOutlineThickness(const DisplayField*);

/* Utility */
sfFloatRect displayField_getGlobalBounds(const DisplayField*);
void displayField_draw(sfRenderWindow*, const DisplayField*);

#endif // DISPLAY_FIELD_H