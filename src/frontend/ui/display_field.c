#include "frontend/ui/display_field.h"

#include <stdlib.h>

struct DisplayField
{
    sfRectangleShape* shape;
    sfText* text;

    sfVector2f textPadding;
    DisplayFieldHorizontalAlignment horizontalAlignment;
    DisplayFieldVerticalAlignment verticalAlignment;
};

static sfFloatRect displayField_getContentBounds(const DisplayField* displayField)
{
    sfVector2f fieldPosition;
    sfVector2f fieldSize;
    sfVector2f fieldOrigin;

    fieldPosition = sfRectangleShape_getPosition(displayField->shape);
    fieldSize = sfRectangleShape_getSize(displayField->shape);
    fieldOrigin = sfRectangleShape_getOrigin(displayField->shape);

    return (sfFloatRect){
        fieldPosition.x - fieldOrigin.x + displayField->textPadding.x,
        fieldPosition.y - fieldOrigin.y + displayField->textPadding.y,
        fieldSize.x - 2.0f * displayField->textPadding.x,
        fieldSize.y - 2.0f * displayField->textPadding.y
    };
}

static void displayField_updateTextPosition(DisplayField* displayField)
{
    sfFloatRect textBounds;
    sfFloatRect contentBounds;

    sfVector2f textOrigin;
    sfVector2f textPosition;

    textBounds = sfText_getLocalBounds(displayField->text);
    contentBounds = displayField_getContentBounds(displayField);

    if (displayField->horizontalAlignment == displayFieldTextAlignLeft)
    {
        textOrigin.x = textBounds.left;
        textPosition.x = contentBounds.left;
    }
    else if (displayField->horizontalAlignment == displayFieldTextAlignRight)
    {
        textOrigin.x = textBounds.left + textBounds.width;
        textPosition.x = contentBounds.left + contentBounds.width;
    }
    else
    {
        textOrigin.x = textBounds.left + textBounds.width / 2.0f;
        textPosition.x = contentBounds.left + contentBounds.width / 2.0f;
    }

    if (displayField->verticalAlignment == displayFieldTextAlignTop)
    {
        textOrigin.y = textBounds.top;
        textPosition.y = contentBounds.top;
    }
    else if (displayField->verticalAlignment == displayFieldTextAlignBottom)
    {
        textOrigin.y = textBounds.top + textBounds.height;
        textPosition.y = contentBounds.top + contentBounds.height;
    }
    else
    {
        textOrigin.y = textBounds.top + textBounds.height / 2.0f;
        textPosition.y = contentBounds.top + contentBounds.height / 2.0f;
    }

    sfText_setOrigin(displayField->text, textOrigin);
    sfText_setPosition(displayField->text, textPosition);
}

DisplayField* displayField_create(void)
{
    DisplayField* displayField;

    displayField = malloc(sizeof(DisplayField));

    displayField->shape = sfRectangleShape_create();
    displayField->text = sfText_create();

    displayField->textPadding = (sfVector2f){0.0f, 0.0f};
    displayField->horizontalAlignment = displayFieldTextAlignCenter;
    displayField->verticalAlignment = displayFieldTextAlignMiddle;

    sfRectangleShape_setSize(displayField->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setPosition(displayField->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setOrigin(displayField->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setFillColor(displayField->shape, sfWhite);
    sfRectangleShape_setOutlineColor(displayField->shape, sfBlack);
    sfRectangleShape_setOutlineThickness(displayField->shape, 0.0f);

    sfText_setString(displayField->text, "");
    sfText_setCharacterSize(displayField->text, 30);
    sfText_setLetterSpacing(displayField->text, 1.0f);
    sfText_setFillColor(displayField->text, sfBlack);

    displayField_updateTextPosition(displayField);

    return displayField;
}

DisplayField* displayField_copy(const DisplayField* displayField)
{
    DisplayField* copy;

    copy = malloc(sizeof(DisplayField));

    copy->shape = sfRectangleShape_copy(displayField->shape);
    copy->text = sfText_copy(displayField->text);

    copy->textPadding = displayField->textPadding;
    copy->horizontalAlignment = displayField->horizontalAlignment;
    copy->verticalAlignment = displayField->verticalAlignment;

    displayField_updateTextPosition(copy);

    return copy;
}

void displayField_destroy(DisplayField* displayField)
{
    sfRectangleShape_destroy(displayField->shape);
    sfText_destroy(displayField->text);
    free(displayField);
}

void displayField_setSize(DisplayField* displayField, sfVector2f size)
{
    sfRectangleShape_setSize(displayField->shape, size);
    displayField_updateTextPosition(displayField);
}

sfVector2f displayField_getSize(const DisplayField* displayField)
{
    return sfRectangleShape_getSize(displayField->shape);
}

void displayField_setPosition(DisplayField* displayField, sfVector2f position)
{
    sfRectangleShape_setPosition(displayField->shape, position);
    displayField_updateTextPosition(displayField);
}

sfVector2f displayField_getPosition(const DisplayField* displayField)
{
    return sfRectangleShape_getPosition(displayField->shape);
}

void displayField_setOrigin(DisplayField* displayField, sfVector2f origin)
{
    sfRectangleShape_setOrigin(displayField->shape, origin);
    displayField_updateTextPosition(displayField);
}

sfVector2f displayField_getOrigin(const DisplayField* displayField)
{
    return sfRectangleShape_getOrigin(displayField->shape);
}

void displayField_setFillColor(DisplayField* displayField, sfColor color)
{
    sfRectangleShape_setFillColor(displayField->shape, color);
}

sfColor displayField_getFillColor(const DisplayField* displayField)
{
    return sfRectangleShape_getFillColor(displayField->shape);
}

void displayField_setTextString(DisplayField* displayField, const char* string)
{
    sfText_setString(displayField->text, string);
    displayField_updateTextPosition(displayField);
}

const char* displayField_getTextString(const DisplayField* displayField)
{
    return sfText_getString(displayField->text);
}

void displayField_setTextFont(DisplayField* displayField, const sfFont* font)
{
    sfText_setFont(displayField->text, font);
    displayField_updateTextPosition(displayField);
}

const sfFont* displayField_getTextFont(const DisplayField* displayField)
{
    return sfText_getFont(displayField->text);
}

void displayField_setCharacterSize(DisplayField* displayField, unsigned int size)
{
    sfText_setCharacterSize(displayField->text, size);
    displayField_updateTextPosition(displayField);
}

unsigned int displayField_getCharacterSize(const DisplayField* displayField)
{
    return sfText_getCharacterSize(displayField->text);
}

void displayField_setLetterSpacing(DisplayField* displayField, float spacing)
{
    sfText_setLetterSpacing(displayField->text, spacing);
    displayField_updateTextPosition(displayField);
}

float displayField_getLetterSpacing(const DisplayField* displayField)
{
    return sfText_getLetterSpacing(displayField->text);
}

void displayField_setTextColor(DisplayField* displayField, sfColor color)
{
    sfText_setFillColor(displayField->text, color);
}

sfColor displayField_getTextColor(const DisplayField* displayField)
{
    return sfText_getFillColor(displayField->text);
}

void displayField_setTextPadding(DisplayField* displayField, sfVector2f padding)
{
    displayField->textPadding = padding;
    displayField_updateTextPosition(displayField);
}

sfVector2f displayField_getTextPadding(const DisplayField* displayField)
{
    return displayField->textPadding;
}

void displayField_setTextHorizontalAlignment(DisplayField* displayField, DisplayFieldHorizontalAlignment alignment)
{
    displayField->horizontalAlignment = alignment;
    displayField_updateTextPosition(displayField);
}

DisplayFieldHorizontalAlignment displayField_getTextHorizontalAlignment(const DisplayField* displayField)
{
    return displayField->horizontalAlignment;
}

void displayField_setTextVerticalAlignment(DisplayField* displayField, DisplayFieldVerticalAlignment alignment)
{
    displayField->verticalAlignment = alignment;
    displayField_updateTextPosition(displayField);
}

DisplayFieldVerticalAlignment displayField_getTextVerticalAlignment(const DisplayField* displayField)
{
    return displayField->verticalAlignment;
}

void displayField_setTextAlignment(
    DisplayField* displayField,
    DisplayFieldHorizontalAlignment horizontalAlignment,
    DisplayFieldVerticalAlignment verticalAlignment
)
{
    displayField->horizontalAlignment = horizontalAlignment;
    displayField->verticalAlignment = verticalAlignment;

    displayField_updateTextPosition(displayField);
}

void displayField_setTexture(DisplayField* displayField, const sfTexture* texture, sfBool resetRect)
{
    sfRectangleShape_setTexture(displayField->shape, texture, resetRect);
}

const sfTexture* displayField_getTexture(const DisplayField* displayField)
{
    return sfRectangleShape_getTexture(displayField->shape);
}

void displayField_setTextureRect(DisplayField* displayField, sfIntRect textureRect)
{
    sfRectangleShape_setTextureRect(displayField->shape, textureRect);
}

sfIntRect displayField_getTextureRect(const DisplayField* displayField)
{
    return sfRectangleShape_getTextureRect(displayField->shape);
}

void displayField_setOutlineColor(DisplayField* displayField, sfColor color)
{
    sfRectangleShape_setOutlineColor(displayField->shape, color);
}

sfColor displayField_getOutlineColor(const DisplayField* displayField)
{
    return sfRectangleShape_getOutlineColor(displayField->shape);
}

void displayField_setOutlineThickness(DisplayField* displayField, float thickness)
{
    sfRectangleShape_setOutlineThickness(displayField->shape, thickness);
    displayField_updateTextPosition(displayField);
}

float displayField_getOutlineThickness(const DisplayField* displayField)
{
    return sfRectangleShape_getOutlineThickness(displayField->shape);
}

sfFloatRect displayField_getGlobalBounds(const DisplayField* displayField)
{
    return sfRectangleShape_getGlobalBounds(displayField->shape);
}

void displayField_draw(sfRenderWindow* window, const DisplayField* displayField)
{
    sfRenderWindow_drawRectangleShape(window, displayField->shape, NULL);
    sfRenderWindow_drawText(window, displayField->text, NULL);
}