#ifndef RESOURCES_H
#define RESOURCES_H

#include <SFML/Graphics.h>


typedef struct Resources Resources;


typedef enum ResourceTextureId
{
    resourceTextureBackground = 0,
    resourceTextureBoard,
    resourceTextureButtons,
    resourceTexturePieces,
    resourceTextureSideboard,
    resourceTextureTextFields,
    resourceTextureTimeBoard,
    resourceTextureTitleBoard,

    resourceTextureCount

} ResourceTextureId;


typedef enum ResourceIconId
{
    resourceIconDraw = 0,
    resourceIconGameLogo,
    resourceIconResign,
    resourceIconSettingWhite,
    resourceIconSettings,
    resourceIconSfmlLogo,
    resourceIconStockfish,

    resourceIconCount

} ResourceIconId;


typedef enum ResourceFontId
{
    resourceFontCinzelMedium = 0,
    resourceFontCinzelRegular,
    resourceFontCinzelSemiBold,

    resourceFontJetBrainsMonoItalic,
    resourceFontJetBrainsMonoLight,
    resourceFontJetBrainsMonoLightItalic,
    resourceFontJetBrainsMonoRegular,
    resourceFontJetBrainsMonoSemiBold,
    resourceFontJetBrainsMonoSemiBoldItalic,

    resourceFontCount

} ResourceFontId;


/* Lifecycle */
Resources* resources_create(void);
void resources_destroy(Resources*);


/* Access */
const sfTexture* resources_getTexture(const Resources*, ResourceTextureId);
const sfTexture* resources_getIcon(const Resources*, ResourceIconId);
const sfFont* resources_getFont(const Resources*, ResourceFontId);


#endif // RESOURCES_H