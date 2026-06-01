#include "frontend/ui/text_field.h"

#include <SFML/Graphics/Color.h>
#include <stdlib.h>
#include <string.h>

#define TEXT_FIELD_MAX_LENGTH 255


typedef struct TextureInfo
{
    sfIntRect idle;
    sfBool idleAvailability;

    sfIntRect hover;
    sfBool hoverAvailability;

    sfIntRect selected;
    sfBool selectedAvailability;

} TextureInfo;

struct TextField
{
    sfRectangleShape* shape;
    sfText* text;

    char textBuffer[TEXT_FIELD_MAX_LENGTH + 1];
    unsigned int textLength;

    sfVector2f textPadding;

    TextureInfo textureInfo;
    TextFieldState state;

    sfColor outlineColor;
    float outlineThickness;

    sfColor selectedOutlineColor;
    float selectedOutlineThickness;
};

static sfBool textField_isTextureRectAvailable(sfIntRect textureRect)
{
    if (textureRect.width == 0 || textureRect.height == 0)
        return sfFalse;

    return sfTrue;
}

static sfIntRect textField_getOptionalIntRect(sfBool available, sfIntRect textureRect)
{
    if (available == sfTrue)
        return textureRect;

    return (sfIntRect){0};
}

static sfFloatRect textField_getContentBounds(const TextField* textField)
{
    sfVector2f fieldPosition;
    sfVector2f fieldSize;
    sfVector2f fieldOrigin;

    fieldPosition = sfRectangleShape_getPosition(textField->shape);
    fieldSize = sfRectangleShape_getSize(textField->shape);
    fieldOrigin = sfRectangleShape_getOrigin(textField->shape);

    return (sfFloatRect){
        fieldPosition.x - fieldOrigin.x + textField->textPadding.x,
        fieldPosition.y - fieldOrigin.y + textField->textPadding.y,
        fieldSize.x - 2.0f * textField->textPadding.x,
        fieldSize.y - 2.0f * textField->textPadding.y
    };
}

static void textField_updateTextPosition(TextField* textField)
{
    sfFloatRect textBounds;
    sfFloatRect contentBounds;

    sfVector2f textOrigin;
    sfVector2f textPosition;

    textBounds = sfText_getLocalBounds(textField->text);
    contentBounds = textField_getContentBounds(textField);

    textOrigin.x = textBounds.left;
    textOrigin.y = textBounds.top + textBounds.height / 2.0f;

    textPosition.x = contentBounds.left;
    textPosition.y = contentBounds.top + contentBounds.height / 2.0f;

    sfText_setOrigin(textField->text, textOrigin);
    sfText_setPosition(textField->text, textPosition);
}

static void textField_applyStateOutline(TextField* textField)
{
    if (textField->state == textFieldSelected)
    {
        sfRectangleShape_setOutlineColor(textField->shape, textField->selectedOutlineColor);
        sfRectangleShape_setOutlineThickness(textField->shape, textField->selectedOutlineThickness);
    }
    else
    {
        sfRectangleShape_setOutlineColor(textField->shape, textField->outlineColor);
        sfRectangleShape_setOutlineThickness(textField->shape, textField->outlineThickness);
    }
}

static void textField_applyStateTextureRect(TextField* textField)
{
    if (textField->state == textFieldSelected)
    {
        if (textField->textureInfo.selectedAvailability == sfTrue)
        {
            sfRectangleShape_setTextureRect(textField->shape, textField->textureInfo.selected);
            return;
        }

        if (textField->textureInfo.hoverAvailability == sfTrue)
        {
            sfRectangleShape_setTextureRect(textField->shape, textField->textureInfo.hover);
            return;
        }

        if (textField->textureInfo.idleAvailability == sfTrue)
            sfRectangleShape_setTextureRect(textField->shape, textField->textureInfo.idle);

        return;
    }

    if (textField->state == textFieldHovered)
    {
        if (textField->textureInfo.hoverAvailability == sfTrue)
        {
            sfRectangleShape_setTextureRect(textField->shape, textField->textureInfo.hover);
            return;
        }

        if (textField->textureInfo.idleAvailability == sfTrue)
            sfRectangleShape_setTextureRect(textField->shape, textField->textureInfo.idle);

        return;
    }

    if (textField->textureInfo.idleAvailability == sfTrue)
        sfRectangleShape_setTextureRect(textField->shape, textField->textureInfo.idle);
}

static void textField_applyStateVisuals(TextField* textField)
{
    textField_applyStateTextureRect(textField);
    textField_applyStateOutline(textField);
    textField_updateTextPosition(textField);
}

static void textField_setState(TextField* textField, TextFieldState state)
{
    textField->state = state;
    textField_applyStateVisuals(textField);
}

static void textField_syncText(TextField* textField)
{
    sfText_setString(textField->text, textField->textBuffer);
    textField_updateTextPosition(textField);
}

static void textField_appendCharacter(TextField* textField, char character)
{
    if (textField->textLength >= TEXT_FIELD_MAX_LENGTH)
        return;

    textField->textBuffer[textField->textLength] = character;
    textField->textLength++;
    textField->textBuffer[textField->textLength] = '\0';

    textField_syncText(textField);
}

static void textField_removeLastCharacter(TextField* textField)
{
    if (textField->textLength == 0)
        return;

    textField->textLength--;
    textField->textBuffer[textField->textLength] = '\0';

    textField_syncText(textField);
}

TextField* textField_create(void)
{
    TextField* textField;

    textField = malloc(sizeof(TextField));

    textField->shape = sfRectangleShape_create();
    textField->text = sfText_create();

    textField->textBuffer[0] = '\0';
    textField->textLength = 0;

    textField->textPadding = (sfVector2f){8.0f, 0.0f};

    textField->textureInfo.idle = (sfIntRect){0};
    textField->textureInfo.idleAvailability = sfFalse;

    textField->textureInfo.hover = (sfIntRect){0};
    textField->textureInfo.hoverAvailability = sfFalse;

    textField->textureInfo.selected = (sfIntRect){0};
    textField->textureInfo.selectedAvailability = sfFalse;

    textField->state = textFieldIdle;

    textField->outlineColor = sfBlack;
    textField->outlineThickness = 0.0f;

    textField->selectedOutlineColor = sfCyan;
    textField->selectedOutlineThickness = 5.0f;

    sfRectangleShape_setSize(textField->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setPosition(textField->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setOrigin(textField->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setFillColor(textField->shape, sfWhite);

    sfText_setString(textField->text, "");
    sfText_setCharacterSize(textField->text, 30);
    sfText_setLetterSpacing(textField->text, 1.0f);
    sfText_setFillColor(textField->text, sfBlack);

    textField_applyStateVisuals(textField);

    return textField;
}

TextField* textField_copy(const TextField* textField)
{
    TextField* copy;

    copy = malloc(sizeof(TextField));

    copy->shape = sfRectangleShape_copy(textField->shape);
    copy->text = sfText_copy(textField->text);

    strcpy(copy->textBuffer, textField->textBuffer);
    copy->textLength = textField->textLength;

    copy->textPadding = textField->textPadding;

    copy->textureInfo = textField->textureInfo;
    copy->state = textField->state;

    copy->outlineColor = textField->outlineColor;
    copy->outlineThickness = textField->outlineThickness;

    copy->selectedOutlineColor = textField->selectedOutlineColor;
    copy->selectedOutlineThickness = textField->selectedOutlineThickness;

    textField_applyStateVisuals(copy);

    return copy;
}

void textField_destroy(TextField* textField)
{
    sfRectangleShape_destroy(textField->shape);
    sfText_destroy(textField->text);
    free(textField);
}

void textField_setSize(TextField* textField, sfVector2f size)
{
    sfRectangleShape_setSize(textField->shape, size);
    textField_updateTextPosition(textField);
}

sfVector2f textField_getSize(const TextField* textField)
{
    return sfRectangleShape_getSize(textField->shape);
}

void textField_setPosition(TextField* textField, sfVector2f position)
{
    sfRectangleShape_setPosition(textField->shape, position);
    textField_updateTextPosition(textField);
}

sfVector2f textField_getPosition(const TextField* textField)
{
    return sfRectangleShape_getPosition(textField->shape);
}

void textField_setOrigin(TextField* textField, sfVector2f origin)
{
    sfRectangleShape_setOrigin(textField->shape, origin);
    textField_updateTextPosition(textField);
}

sfVector2f textField_getOrigin(const TextField* textField)
{
    return sfRectangleShape_getOrigin(textField->shape);
}

void textField_setFillColor(TextField* textField, sfColor color)
{
    sfRectangleShape_setFillColor(textField->shape, color);
}

sfColor textField_getFillColor(const TextField* textField)
{
    return sfRectangleShape_getFillColor(textField->shape);
}

void textField_setTextString(TextField* textField, const char* string)
{
    unsigned int i;

    i = 0;

    while (string[i] != '\0' && i < TEXT_FIELD_MAX_LENGTH)
    {
        textField->textBuffer[i] = string[i];
        i++;
    }

    textField->textBuffer[i] = '\0';
    textField->textLength = i;

    textField_syncText(textField);
}

const char* textField_getTextString(const TextField* textField)
{
    return textField->textBuffer;
}

void textField_clear(TextField* textField)
{
    textField->textBuffer[0] = '\0';
    textField->textLength = 0;

    textField_syncText(textField);
}

void textField_setTextFont(TextField* textField, const sfFont* font)
{
    sfText_setFont(textField->text, font);
    textField_updateTextPosition(textField);
}

const sfFont* textField_getTextFont(const TextField* textField)
{
    return sfText_getFont(textField->text);
}

void textField_setCharacterSize(TextField* textField, unsigned int size)
{
    sfText_setCharacterSize(textField->text, size);
    textField_updateTextPosition(textField);
}

unsigned int textField_getCharacterSize(const TextField* textField)
{
    return sfText_getCharacterSize(textField->text);
}

void textField_setLetterSpacing(TextField* textField, float spacing)
{
    sfText_setLetterSpacing(textField->text, spacing);
    textField_updateTextPosition(textField);
}

float textField_getLetterSpacing(const TextField* textField)
{
    return sfText_getLetterSpacing(textField->text);
}

void textField_setTextColor(TextField* textField, sfColor color)
{
    sfText_setFillColor(textField->text, color);
}

sfColor textField_getTextColor(const TextField* textField)
{
    return sfText_getFillColor(textField->text);
}

void textField_setTextPadding(TextField* textField, sfVector2f padding)
{
    textField->textPadding = padding;
    textField_updateTextPosition(textField);
}

sfVector2f textField_getTextPadding(const TextField* textField)
{
    return textField->textPadding;
}

void textField_setTexture(TextField* textField, const sfTexture* texture, sfBool resetRect)
{
    sfRectangleShape_setTexture(textField->shape, texture, resetRect);
}

const sfTexture* textField_getTexture(const TextField* textField)
{
    return sfRectangleShape_getTexture(textField->shape);
}

void textField_setTextureRect_onIdle(TextField* textField, sfIntRect idle)
{
    textField->textureInfo.idle = idle;
    textField->textureInfo.idleAvailability = textField_isTextureRectAvailable(idle);

    textField_applyStateVisuals(textField);
}

sfIntRect textField_getTextureRect_onIdle(const TextField* textField)
{
    return textField_getOptionalIntRect(
        textField->textureInfo.idleAvailability,
        textField->textureInfo.idle
    );
}

void textField_setTextureRect_onHover(TextField* textField, sfIntRect hover)
{
    textField->textureInfo.hover = hover;
    textField->textureInfo.hoverAvailability = textField_isTextureRectAvailable(hover);

    textField_applyStateVisuals(textField);
}

sfIntRect textField_getTextureRect_onHover(const TextField* textField)
{
    return textField_getOptionalIntRect(
        textField->textureInfo.hoverAvailability,
        textField->textureInfo.hover
    );
}

void textField_setTextureRect_onSelected(TextField* textField, sfIntRect selected)
{
    textField->textureInfo.selected = selected;
    textField->textureInfo.selectedAvailability = textField_isTextureRectAvailable(selected);

    textField_applyStateVisuals(textField);
}

sfIntRect textField_getTextureRect_onSelected(const TextField* textField)
{
    return textField_getOptionalIntRect(
        textField->textureInfo.selectedAvailability,
        textField->textureInfo.selected
    );
}

void textField_setOutlineColor(TextField* textField, sfColor color)
{
    textField->outlineColor = color;
    textField_applyStateVisuals(textField);
}

sfColor textField_getOutlineColor(const TextField* textField)
{
    return textField->outlineColor;
}

void textField_setOutlineThickness(TextField* textField, float thickness)
{
    textField->outlineThickness = thickness;
    textField_applyStateVisuals(textField);
}

float textField_getOutlineThickness(const TextField* textField)
{
    return textField->outlineThickness;
}

void textField_setSelectedOutlineColor(TextField* textField, sfColor color)
{
    textField->selectedOutlineColor = color;
    textField_applyStateVisuals(textField);
}

sfColor textField_getSelectedOutlineColor(const TextField* textField)
{
    return textField->selectedOutlineColor;
}

void textField_setSelectedOutlineThickness(TextField* textField, float thickness)
{
    textField->selectedOutlineThickness = thickness;
    textField_applyStateVisuals(textField);
}

float textField_getSelectedOutlineThickness(const TextField* textField)
{
    return textField->selectedOutlineThickness;
}

TextFieldState textField_getState(const TextField* textField)
{
    return textField->state;
}

sfBool textField_isSelected(const TextField* textField)
{
    if (textField->state == textFieldSelected)
        return sfTrue;

    return sfFalse;
}

void textField_select(TextField* textField)
{
    textField_setState(textField, textFieldSelected);
}

void textField_deselect(TextField* textField)
{
    textField_setState(textField, textFieldIdle);
}

sfBool textField_containsPoint(const TextField* textField, sfVector2i point)
{
    sfFloatRect bounds;

    bounds = sfRectangleShape_getGlobalBounds(textField->shape);

    if ((float)point.x < bounds.left)
        return sfFalse;

    if ((float)point.x > bounds.left + bounds.width)
        return sfFalse;

    if ((float)point.y < bounds.top)
        return sfFalse;

    if ((float)point.y > bounds.top + bounds.height)
        return sfFalse;

    return sfTrue;
}

void textField_updateMouse(TextField* textField, const Mouse* mouse)
{
    sfBool mouseInside;

    mouseInside = textField_containsPoint(textField, mouse_getPosition(mouse));

    if (mouse_wasJustPressed(mouse) == sfTrue && mouseInside == sfTrue)
    {
        if (textField->state == textFieldSelected)
            textField_setState(textField, textFieldHovered);
        else
            textField_setState(textField, textFieldSelected);

        return;
    }

    if (textField->state == textFieldSelected)
        return;

    if (mouseInside == sfTrue)
        textField_setState(textField, textFieldHovered);
    else
        textField_setState(textField, textFieldIdle);
}

void textField_updateKeyboard(TextField* textField, const sfEvent* event)
{
    if (textField->state != textFieldSelected)
        return;

    if (event->type == sfEvtKeyPressed)
    {
        if (event->key.code == sfKeyReturn)
        {
            textField_deselect(textField);
            return;
        }

        if (event->key.code == sfKeyBack)
        {
            textField_removeLastCharacter(textField);
            return;
        }
    }

    if (event->type == sfEvtTextEntered)
    {
        if (event->text.unicode >= 32 && event->text.unicode <= 126)
            textField_appendCharacter(textField, (char)event->text.unicode);
    }
}

sfFloatRect textField_getGlobalBounds(const TextField* textField)
{
    return sfRectangleShape_getGlobalBounds(textField->shape);
}

void textField_draw(sfRenderWindow* window, const TextField* textField)
{
    sfRenderWindow_drawRectangleShape(window, textField->shape, NULL);
    sfRenderWindow_drawText(window, textField->text, NULL);
}