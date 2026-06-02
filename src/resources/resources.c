#include "resources/resources.h"

#include <stdlib.h>


struct Resources
{
    sfTexture* textures[resourceTextureCount];
    sfTexture* icons[resourceIconCount];
    sfFont* fonts[resourceFontCount];
};


static const char* const texturePaths[resourceTextureCount] =
{
    [resourceTextureBackground] = "media/textures/background.png",
    [resourceTextureBoard] = "media/textures/board.png",
    [resourceTextureButtons] = "media/textures/buttons.png",
    [resourceTexturePieces] = "media/textures/pieces.png",
    [resourceTextureSideboard] = "media/textures/sideboard.png",
    [resourceTextureTextFields] = "media/textures/text_fields.png",
    [resourceTextureTimeBoard] = "media/textures/time_board.png",
    [resourceTextureTitleBoard] = "media/textures/title_board.png"
};


static const char* const iconPaths[resourceIconCount] =
{
    [resourceIconDraw] = "media/icons/draw.png",
    [resourceIconGameLogo] = "media/icons/game_logo.png",
    [resourceIconResign] = "media/icons/resign.png",
    [resourceIconSettingWhite] = "media/icons/setting_white.png",
    [resourceIconSettings] = "media/icons/settings.png",
    [resourceIconSfmlLogo] = "media/icons/sfml_logo.png",
    [resourceIconStockfish] = "media/icons/stockfish.png"
};


static const char* const fontPaths[resourceFontCount] =
{
    [resourceFontCinzelMedium] = "media/fonts/Cinzel-Medium.ttf",
    [resourceFontCinzelRegular] = "media/fonts/Cinzel-Regular.ttf",
    [resourceFontCinzelSemiBold] = "media/fonts/Cinzel-SemiBold.ttf",

    [resourceFontJetBrainsMonoItalic] = "media/fonts/JetBrainsMono-Italic.ttf",
    [resourceFontJetBrainsMonoLight] = "media/fonts/JetBrainsMono-Light.ttf",
    [resourceFontJetBrainsMonoLightItalic] = "media/fonts/JetBrainsMono-LightItalic.ttf",
    [resourceFontJetBrainsMonoRegular] = "media/fonts/JetBrainsMono-Regular.ttf",
    [resourceFontJetBrainsMonoSemiBold] = "media/fonts/JetBrainsMono-SemiBold.ttf",
    [resourceFontJetBrainsMonoSemiBoldItalic] = "media/fonts/JetBrainsMono-SemiBoldItalic.ttf"
};


static void resources_clear(Resources* resources)
{
    int i;

    for (i = 0; i < resourceTextureCount; i++)
        resources->textures[i] = NULL;

    for (i = 0; i < resourceIconCount; i++)
        resources->icons[i] = NULL;

    for (i = 0; i < resourceFontCount; i++)
        resources->fonts[i] = NULL;
}


static sfTexture* resources_loadSmoothTexture(const char* path)
{
    sfTexture* texture;

    texture = sfTexture_createFromFile(path, NULL);

    if (texture == NULL)
        return NULL;

    sfTexture_setSmooth(texture, sfTrue);

    return texture;
}


static sfBool resources_loadTextures(Resources* resources)
{
    int i;

    for (i = 0; i < resourceTextureCount; i++)
    {
        resources->textures[i] = resources_loadSmoothTexture(texturePaths[i]);

        if (resources->textures[i] == NULL)
            return sfFalse;
    }

    return sfTrue;
}


static sfBool resources_loadIcons(Resources* resources)
{
    int i;

    for (i = 0; i < resourceIconCount; i++)
    {
        resources->icons[i] = resources_loadSmoothTexture(iconPaths[i]);

        if (resources->icons[i] == NULL)
            return sfFalse;
    }

    return sfTrue;
}


static sfBool resources_loadFonts(Resources* resources)
{
    int i;

    for (i = 0; i < resourceFontCount; i++)
    {
        resources->fonts[i] = sfFont_createFromFile(fontPaths[i]);

        if (resources->fonts[i] == NULL)
            return sfFalse;
    }

    return sfTrue;
}


Resources* resources_create(void)
{
    Resources* resources;

    resources = malloc(sizeof(Resources));

    if (resources == NULL)
        return NULL;

    resources_clear(resources);

    if (resources_loadTextures(resources) == sfFalse)
    {
        resources_destroy(resources);
        return NULL;
    }

    if (resources_loadIcons(resources) == sfFalse)
    {
        resources_destroy(resources);
        return NULL;
    }

    if (resources_loadFonts(resources) == sfFalse)
    {
        resources_destroy(resources);
        return NULL;
    }

    return resources;
}


void resources_destroy(Resources* resources)
{
    int i;

    if (resources == NULL)
        return;

    for (i = 0; i < resourceTextureCount; i++)
    {
        if (resources->textures[i] != NULL)
            sfTexture_destroy(resources->textures[i]);
    }

    for (i = 0; i < resourceIconCount; i++)
    {
        if (resources->icons[i] != NULL)
            sfTexture_destroy(resources->icons[i]);
    }

    for (i = 0; i < resourceFontCount; i++)
    {
        if (resources->fonts[i] != NULL)
            sfFont_destroy(resources->fonts[i]);
    }

    free(resources);
}


const sfTexture* resources_getTexture(const Resources* resources, ResourceTextureId id)
{
    if (resources == NULL)
        return NULL;

    if (id < 0 || id >= resourceTextureCount)
        return NULL;

    return resources->textures[id];
}


const sfTexture* resources_getIcon(const Resources* resources, ResourceIconId id)
{
    if (resources == NULL)
        return NULL;

    if (id < 0 || id >= resourceIconCount)
        return NULL;

    return resources->icons[id];
}


const sfFont* resources_getFont(const Resources* resources, ResourceFontId id)
{
    if (resources == NULL)
        return NULL;

    if (id < 0 || id >= resourceFontCount)
        return NULL;

    return resources->fonts[id];
}