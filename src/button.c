#include "button.h"

#include <stdlib.h>

typedef struct TextureInfo
{
    sfIntRect idle;
    sfBool idleAvailability;

    sfIntRect hover;
    sfBool hoverAvailability;

    sfIntRect press;
    sfBool pressAvailability;

} TextureInfo;

struct Button
{
    sfRectangleShape* shape;
    sfText* text;

    TextureInfo textureInfo;
    ButtonState state;

    buttonAction action;
    void* actionData;

    sfBool pressStartedInside;
};

static sfBool button_isTextureRectAvailable(sfIntRect textureRect)
{
    if (textureRect.width == 0 || textureRect.height == 0)
        return sfFalse;

    return sfTrue;
}

static sfIntRect button_getOptionalIntRect(sfBool available, sfIntRect textureRect)
{
    if (available)
        return textureRect;

    return (sfIntRect){0};
}

static void button_updateTextPosition(Button* button)
{
    sfFloatRect textBounds;
    sfVector2f buttonPosition;
    sfVector2f buttonSize;
    sfVector2f buttonOrigin;

    textBounds = sfText_getLocalBounds(button->text);

    buttonPosition = sfRectangleShape_getPosition(button->shape);
    buttonSize = sfRectangleShape_getSize(button->shape);
    buttonOrigin = sfRectangleShape_getOrigin(button->shape);

    sfText_setOrigin(
        button->text,
        (sfVector2f){
            textBounds.left + textBounds.width / 2.0f,
            textBounds.top + textBounds.height / 2.0f
        }
    );

    sfText_setPosition(
        button->text,
        (sfVector2f){
            buttonPosition.x + buttonSize.x / 2.0f - buttonOrigin.x,
            buttonPosition.y + buttonSize.y / 2.0f - buttonOrigin.y
        }
    );
}

static void button_applyStateTextureRect(Button* button)
{
    if (button->state == buttonPressed)
    {
        if (button->textureInfo.pressAvailability)
        {
            sfRectangleShape_setTextureRect(button->shape, button->textureInfo.press);
            return;
        }

        if (button->textureInfo.hoverAvailability)
        {
            sfRectangleShape_setTextureRect(button->shape, button->textureInfo.hover);
            return;
        }

        if (button->textureInfo.idleAvailability)
            sfRectangleShape_setTextureRect(button->shape, button->textureInfo.idle);

        return;
    }

    if (button->state == buttonHovered)
    {
        if (button->textureInfo.hoverAvailability)
        {
            sfRectangleShape_setTextureRect(button->shape, button->textureInfo.hover);
            return;
        }

        if (button->textureInfo.idleAvailability)
            sfRectangleShape_setTextureRect(button->shape, button->textureInfo.idle);

        return;
    }

    if (button->textureInfo.idleAvailability)
        sfRectangleShape_setTextureRect(button->shape, button->textureInfo.idle);
}

static void button_setState(Button* button, ButtonState state)
{
    button->state = state;
    button_applyStateTextureRect(button);
}

Button* button_create(void)
{
    Button* button;

    button = malloc(sizeof(Button));

    button->shape = sfRectangleShape_create();
    button->text = sfText_create();

    button->textureInfo.idle = (sfIntRect){0};
    button->textureInfo.idleAvailability = sfFalse;

    button->textureInfo.hover = (sfIntRect){0};
    button->textureInfo.hoverAvailability = sfFalse;

    button->textureInfo.press = (sfIntRect){0};
    button->textureInfo.pressAvailability = sfFalse;

    button->state = buttonIdle;

    button->action = NULL;
    button->actionData = NULL;

    button->pressStartedInside = sfFalse;

    sfRectangleShape_setSize(button->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setPosition(button->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setOrigin(button->shape, (sfVector2f){0.0f, 0.0f});
    sfRectangleShape_setFillColor(button->shape, sfWhite);
    sfRectangleShape_setOutlineColor(button->shape, sfBlack);
    sfRectangleShape_setOutlineThickness(button->shape, 0.0f);

    sfText_setString(button->text, "");
    sfText_setCharacterSize(button->text, 30);
    sfText_setLetterSpacing(button->text, 1.0f);
    sfText_setFillColor(button->text, sfBlack);

    button_updateTextPosition(button);

    return button;
}

Button* button_copy(const Button* button)
{
    Button* copy;

    copy = malloc(sizeof(Button));

    copy->shape = sfRectangleShape_copy(button->shape);
    copy->text = sfText_copy(button->text);

    copy->textureInfo = button->textureInfo;
    copy->state = button->state;

    copy->action = button->action;
    copy->actionData = button->actionData;

    copy->pressStartedInside = sfFalse;

    button_updateTextPosition(copy);
    button_applyStateTextureRect(copy);

    return copy;
}

void button_destroy(Button* button)
{
    sfRectangleShape_destroy(button->shape);
    sfText_destroy(button->text);
    free(button);
}

void button_setSize(Button* button, sfVector2f size)
{
    sfRectangleShape_setSize(button->shape, size);
    button_updateTextPosition(button);
}

sfVector2f button_getSize(const Button* button)
{
    return sfRectangleShape_getSize(button->shape);
}

void button_setPosition(Button* button, sfVector2f position)
{
    sfRectangleShape_setPosition(button->shape, position);
    button_updateTextPosition(button);
}

sfVector2f button_getPosition(const Button* button)
{
    return sfRectangleShape_getPosition(button->shape);
}

void button_setOrigin(Button* button, sfVector2f origin)
{
    sfRectangleShape_setOrigin(button->shape, origin);
    button_updateTextPosition(button);
}

sfVector2f button_getOrigin(const Button* button)
{
    return sfRectangleShape_getOrigin(button->shape);
}

void button_setFillColor(Button* button, sfColor color)
{
    sfRectangleShape_setFillColor(button->shape, color);
}

sfColor button_getFillColor(const Button* button)
{
    return sfRectangleShape_getFillColor(button->shape);
}

void button_setTextString(Button* button, const char* string)
{
    sfText_setString(button->text, string);
    button_updateTextPosition(button);
}

const char* button_getTextString(const Button* button)
{
    return sfText_getString(button->text);
}

void button_setTextFont(Button* button, const sfFont* font)
{
    sfText_setFont(button->text, font);
    button_updateTextPosition(button);
}

const sfFont* button_getTextFont(const Button* button)
{
    return sfText_getFont(button->text);
}

void button_setCharacterSize(Button* button, unsigned int size)
{
    sfText_setCharacterSize(button->text, size);
    button_updateTextPosition(button);
}

unsigned int button_getCharacterSize(const Button* button)
{
    return sfText_getCharacterSize(button->text);
}

void button_setLetterSpacing(Button* button, float spacing)
{
    sfText_setLetterSpacing(button->text, spacing);
    button_updateTextPosition(button);
}

float button_getLetterSpacing(const Button* button)
{
    return sfText_getLetterSpacing(button->text);
}

void button_setTextColor(Button* button, sfColor color)
{
    sfText_setFillColor(button->text, color);
}

sfColor button_getTextColor(const Button* button)
{
    return sfText_getFillColor(button->text);
}

void button_setTexture(Button* button, const sfTexture* texture, sfBool resetRect)
{
    sfRectangleShape_setTexture(button->shape, texture, resetRect);
}

const sfTexture* button_getTexture(const Button* button)
{
    return sfRectangleShape_getTexture(button->shape);
}

void button_setTextureRect_onIdle(Button* button, sfIntRect idle)
{
    button->textureInfo.idle = idle;
    button->textureInfo.idleAvailability = button_isTextureRectAvailable(idle);

    button_applyStateTextureRect(button);
}

sfIntRect button_getTextureRect_onIdle(const Button* button)
{
    return button_getOptionalIntRect(
        button->textureInfo.idleAvailability,
        button->textureInfo.idle
    );
}

void button_setTextureRect_onHover(Button* button, sfIntRect hover)
{
    button->textureInfo.hover = hover;
    button->textureInfo.hoverAvailability = button_isTextureRectAvailable(hover);

    button_applyStateTextureRect(button);
}

sfIntRect button_getTextureRect_onHover(const Button* button)
{
    return button_getOptionalIntRect(
        button->textureInfo.hoverAvailability,
        button->textureInfo.hover
    );
}

void button_setTextureRect_onPress(Button* button, sfIntRect press)
{
    button->textureInfo.press = press;
    button->textureInfo.pressAvailability = button_isTextureRectAvailable(press);

    button_applyStateTextureRect(button);
}

sfIntRect button_getTextureRect_onPress(const Button* button)
{
    return button_getOptionalIntRect(
        button->textureInfo.pressAvailability,
        button->textureInfo.press
    );
}

void button_setOutlineColor(Button* button, sfColor color)
{
    sfRectangleShape_setOutlineColor(button->shape, color);
}

sfColor button_getOutlineColor(const Button* button)
{
    return sfRectangleShape_getOutlineColor(button->shape);
}

void button_setOutlineThickness(Button* button, float thickness)
{
    sfRectangleShape_setOutlineThickness(button->shape, thickness);
    button_updateTextPosition(button);
}

float button_getOutlineThickness(const Button* button)
{
    return sfRectangleShape_getOutlineThickness(button->shape);
}

ButtonState button_getState(const Button* button)
{
    return button->state;
}

void button_setAction(Button* button, buttonAction action, void* actionData)
{
    button->action = action;
    button->actionData = actionData;
}

buttonAction button_getAction(const Button* button)
{
    return button->action;
}

void* button_getActionData(const Button* button)
{
    return button->actionData;
}

void button_triggerAction(Button* button)
{
    if (button->action != NULL)
        button->action(button->actionData);
}

sfBool button_containsPoint(const Button* button, sfVector2i point)
{
    sfFloatRect bounds;

    bounds = sfRectangleShape_getGlobalBounds(button->shape);

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

void button_updateMouse(Button* button, const Mouse* mouse)
{
    sfBool mouseInside;

    mouseInside = button_containsPoint(button, mouse_getPosition(mouse));

    if (mouse_wasJustPressed(mouse))
        button->pressStartedInside = mouseInside;

    if (!mouseInside)
    {
        button_setState(button, buttonIdle);

        if (mouse_wasJustReleased(mouse))
            button->pressStartedInside = sfFalse;

        return;
    }

    if (mouse_isPressed(mouse))
    {
        if (button->pressStartedInside)
            button_setState(button, buttonPressed);

        return;
    }

    if (mouse_wasJustReleased(mouse))
    {
        if (button->pressStartedInside)
            button_triggerAction(button);

        button->pressStartedInside = sfFalse;
    }

    button_setState(button, buttonHovered);
}

sfFloatRect button_getGlobalBounds(const Button* button)
{
    return sfRectangleShape_getGlobalBounds(button->shape);
}

void button_draw(sfRenderWindow* window, const Button* button)
{
    sfRenderWindow_drawRectangleShape(window, button->shape, NULL);
    sfRenderWindow_drawText(window, button->text, NULL);
}