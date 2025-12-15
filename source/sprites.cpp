#include "sprites.h"
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <cmath>
#include <algorithm>
#include <nf_lib.h>
#include <iostream>
#include "misc.h"
#include "objectpool.h"


#define MaxTemplates 256
#define MaxSprites 128
#define MaxRotScaleGroups 32
#define MaxPalettesPerScreen 15
#define MaxPalettes 64






enum class AnimationType {
    None,
    Idle,
    Walk,
    Shoot,
    Fix,
    Die,
    Selected,
    UnSelected,
    VisitedSelected,
    VisitedUnSelected
};






class SpriteTemplate {
    public:
        Identifier identity;
        std::string name;
        std::optional<Identifier> screenOvram;
        std::optional<Identifier> screen1vram;
        int width, height;
        bool isAnimated;
        std::map<std::string, std::pair<std::vector<int>, int>> animations;


        SpriteTemplate(Identifier identity, const std::string& filename, int width, int height,bool isAnimated) : identity(identity), name(filename), width(width), height(height), isAnimated(isAnimated) {    
            NF_LoadSpriteGfx((std::string("sprites/")+std::string(filename)).c_str(), identity.id, width, height);
        }


        void AddAnimation(const std::string& animationName, const std::vector<int>& frames, int frameDelay) {
            animations[animationName] = std::make_pair(frames, frameDelay);
        }

        void LoadTemplateToVRAM(int screen, Identifier vramIdentity){
            std::optional<Identifier>& vramID = (screen == 0) ? screenOvram: screen1vram;
            if (vramID.has_value()) return; 
            vramID = vramIdentity;
            vramID.value().screen = screen;
            NF_VramSpriteGfx(screen,identity.id,vramID.value().id,isAnimated);
            //std::cout << "Template: " << name << ", ID: " << identity.id << ", GFXID: " << vramID.value().id << std::endl;
        }

        
        void UnloadTemplateFromVRAM(int screen){
            std::optional<Identifier>& vramID = (screen == 0) ? screenOvram: screen1vram; 
            NF_FreeSpriteGfx(screen, vramID.value().id);
            vramID.reset();
        }


        bool IsLoadedInVRAM(int screen) {
            std::optional<Identifier>& vramID = (screen == 0) ? screenOvram : screen1vram;
            return vramID.has_value();
        }


        Identifier GetTemplateVRAM(int screen){
            return (screen == 0) ? screenOvram.value(): screen1vram.value(); 
        }

        
        ~SpriteTemplate() {
            NF_UnloadSpriteGfx(identity.id);
        }
};


class SpritePalette{
    public:
        Identifier identity;
        std::string name;
        std::optional<Identifier> screen0vram;
        std::optional<Identifier> screen1vram;

        SpritePalette(Identifier identity, const std::string& paletteName): identity(identity), name(paletteName){
            //std::cout << "Palette: " << name << ", ID: " << identity.id << std::endl;
            NF_LoadSpritePal(name.c_str(), identity.id);
        }

        void LoadPaletteToVRAM(int screen, Identifier vramIdentity){
            std::optional<Identifier>& vramID = (screen == 0) ? screen0vram: screen1vram;
            if (vramID.has_value()) return; 
            vramID = vramIdentity;
            vramID.value().screen = screen;
            //std::cout << "Palette: " << name << ", ID: " << identity.id << ", GFXID: " << vramID.value().id << std::endl;
            NF_VramSpritePal(screen, identity.id, vramID.value().id);
        }

        bool IsLoadedInVRAM(int screen) {
            std::optional<Identifier>& vramID = (screen == 0) ? screen0vram : screen1vram;
            return vramID.has_value();
        }

        void UnloadPaletteFromVRAM(int screen){
            std::optional<Identifier>& vramID = (screen == 0) ? screen0vram: screen1vram; 
            vramID.reset();
        }

        Identifier GetPaletteVRAM(int screen){
            return (screen == 0) ? screen0vram.value(): screen1vram.value(); 
        }

        ~SpritePalette(){
            NF_UnloadSpritePal(identity.id);
        }
};


class SpriteInstance {
    private:
        // Animation state
        std::string currentAnim;
        std::vector<int> currentFrames;  
        int frameTimer = 0;
        int frameDelay = 0;
    protected:              
    public:
        Identifier identity;
        int currentFrame = 0;
        int screen;
        int gfxID;
        int plttID;
        Pos position;
        int width, height;
        bool isAnimated;
        SpriteTemplate* template_;

        SpriteInstance(Identifier identity, int screen, SpriteTemplate* tmpl, SpritePalette* pltt, Pos pos): identity(identity), screen(screen), gfxID(((screen ==0) ? tmpl->screenOvram.value() : tmpl->screen1vram.value()).id), plttID(((screen == 0) ? pltt-> screen0vram.value() : pltt -> screen1vram.value()).id), position(pos), width(tmpl->width), height(tmpl->height), isAnimated(tmpl->isAnimated), template_(tmpl) { 
            //std::cout << "Sprite: " << tmpl->name << ", GFXIDs: " << gfxID << ", ID: " << identity.id << std::endl;
            //std::cout << ", Frames: " << NF_SPR256VRAM[screen][gfxID].lastframe << std::endl;
            this->identity.screen = screen;
            NF_CreateSprite(screen, identity.id, gfxID, plttID, pos.x, pos.y);
        }
    
        ~SpriteInstance(){
            NF_ShowSprite(screen, identity.id, false);
            NF_DeleteSprite(screen, identity.id);
        }

        void SetAnimation(const std::string& animationName) {
            auto animIt = template_->animations.find(animationName);
            if (animIt == template_->animations.end()) return;
            
            currentAnim = animationName;
            currentFrame = 0;
            frameTimer = 0;  
            

            currentFrames = std::vector<int>();
            currentFrames = animIt->second.first;
            frameDelay = animIt->second.second;
            NF_SpriteFrame(screen, identity.id, currentFrames[0]);
        }


        void SetLayer(int layer){
            NF_SpriteLayer(screen, identity.id, layer);
        }

        void Move(Pos position){
            NF_MoveSprite(screen, identity.id, position.x, position.y);
        }

        void HFlip(bool isFlipped) {
            NF_HflipSprite(screen, identity.id, isFlipped);
        }

        bool GetHFlip(){
            return NF_GetSpriteHflip(screen,identity.id);
        }

        void VFlip(bool isFlipped) {
            NF_VflipSprite(screen, identity.id, isFlipped);
        }

        bool GetVFlip(){
            return NF_GetSpriteVflip(screen, identity.id);
        }

        void SetAnimationFrame(int frame){
            NF_SpriteFrame(screen, identity.id, frame);
        }

        void Show(bool show){
            NF_ShowSprite(screen, identity.id, show);
        }

        void BindToRotScaleGroup(int group, bool doubleSize){
            NF_EnableSpriteRotScale(screen, identity.id, group, doubleSize);
        }

        void RemoveFromRotScaleGroup(){
            NF_DisableSpriteRotScale(screen, identity.id);
        }

        void Update() {
            if (currentFrames.empty()) return;
            
            frameTimer++;
            if (frameTimer >= frameDelay) {
                frameTimer = 0;
                currentFrame = (currentFrame + 1) % currentFrames.size();
                SetAnimationFrame(currentFrames[currentFrame]);
            }
        }

        bool operator==(SpriteInstance* spriteToCompare) const {
            if(spriteToCompare->screen == screen && spriteToCompare->identity == identity){
                return true;
            }
            return false;
        }

};


struct RotScaleGroup
{
    Identifier identity;
    int screen;
    float rotation;
    float scaleX;
    float scaleY;
    std::vector<Identifier> spritesInGroup;
    RotScaleGroup(Identifier identity, int screen, float rotation, float scaleX, float scaleY): identity(identity), screen(screen), rotation(rotation), scaleX(scaleX), scaleY(scaleY){
        this->identity.screen = screen;
    }
    void ApplyRotationAndScale(){
        NF_SpriteRotScale(screen, identity.id, round(rotation * 1.422222f), round(scaleX * 2.56f), round(scaleY * 2.56));
    }
    void AddSpritetoGroup(Identifier spriteIdentity){
        spritesInGroup.push_back(spriteIdentity);
    }
    void RemoveSpriteFromGroup(Identifier spriteIdentity){
        spritesInGroup.erase(std::remove(spritesInGroup.begin(), spritesInGroup.end(), spriteIdentity), spritesInGroup.end());
    }
};



struct ScreenRegistry
{
    ObjectPool<SpriteInstance, MaxSprites> sprites;
    ObjectPool<RotScaleGroup, MaxRotScaleGroups> rotScaleGroups;
    ObjectPool<Identifier, MaxSprites> templatesInVRAM;
    ObjectPool<Identifier, MaxPalettesPerScreen> palettesInVRAM;
};


class SpriteManager {
    private:
        SpriteManager() = default;
        
        SpriteManager(const SpriteManager&) = delete;
        SpriteManager& operator=(const SpriteManager&) = delete;
    public:
        
        ScreenRegistry screen0;
        ScreenRegistry screen1;

        std::map<std::string, Identifier> templateRegistry;
        ObjectPool<SpriteTemplate, MaxTemplates> templates;
        std::map<std::string, Identifier> paletteRegistry;
        ObjectPool<SpritePalette, MaxPalettes> palettes;


        static inline ScreenRegistry& GetScreenRegistry(int screen){
            return (screen == 0) ? Instance().screen0 : Instance().screen1;
        }
    

        SpriteTemplate* GetSpriteTemplate(const std::string& templateName){
            bool exists = !(templateRegistry.find(templateName) == templateRegistry.end());
            SpriteTemplate* tmpl = templates.get(templateRegistry[templateName]);
            if(tmpl == nullptr || !exists){
                std::cout << "Sprite Template:" << templateName << " doesn't exist" << std::endl;
                return nullptr;
            }
            return tmpl;
        }


        SpritePalette* GetSpritePalette(const std::string& paletteName){
            bool exists = !(paletteRegistry.find(paletteName) == paletteRegistry.end());
            SpritePalette* pltt = palettes.get(paletteRegistry[paletteName]);
            if(pltt == nullptr || !exists) {
                std::cout << "Sprite Palette: " << paletteName << " doesn't exist" << std::endl;
                return nullptr;
            }
            return pltt;
        }


        SpriteInstance* GetSpriteInstance(Identifier identity){
            if (!identity.screen.has_value()) return nullptr; 
            ScreenRegistry& registry = GetScreenRegistry(identity.screen.value());
            return registry.sprites.get(identity);
        }


        RotScaleGroup* GetRotScaleGroup(Identifier identity){
            if (!identity.screen.has_value()) return nullptr;
            ScreenRegistry& registry = GetScreenRegistry(identity.screen.value());
            return registry.rotScaleGroups.get(identity);
        }


        static SpriteManager& Instance() {
            static SpriteManager instance;  
            return instance;
        }
    };

    SpriteManager& spriteManager = SpriteManager::Instance();

        Identifier CM_CreateSpriteTemplate(const std::string& name, int width, int height, bool isAnimated) {
            SpriteTemplate* tmpl = spriteManager.templates.create(name.c_str(), width, height, isAnimated);
            if(tmpl != nullptr){
                spriteManager.templateRegistry[name] = tmpl->identity;
            }
            else{
                std::cout << "Could not create Sprite Template: " << name << ", Limit Exceeded" << std::endl;
            }
            return tmpl->identity;
        }


        void CM_LoadSpriteTemplateToVRAM(int screen, const std::string& templateName){
            SpriteTemplate* tmpl = spriteManager.GetSpriteTemplate(templateName);
            if(tmpl == nullptr) return;
            if (tmpl->IsLoadedInVRAM(screen)) {
                std::cout << templateName << " already in VRAM on screen " << screen << "\n";
                return;
            }
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(screen);

            Identifier* vramId = registry.templatesInVRAM.create();
            if (vramId != nullptr) {
                tmpl->LoadTemplateToVRAM(screen, *vramId);
            } else {
                std::cout << "VRAM Template Pool Full! Cannot load " << templateName << "\n";
            }
        }


        void CM_UnloadSpriteTemplateFromVRAM(int screen, const std::string& templateName){
            SpriteTemplate* tmpl = spriteManager.GetSpriteTemplate(templateName);
            if(tmpl == nullptr) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(screen);
            registry.templatesInVRAM.remove(tmpl->GetTemplateVRAM(screen));
            tmpl->UnloadTemplateFromVRAM(screen);
        }


        void CM_DeleteSpriteTemplate(const std::string& templateName){
            SpriteTemplate* tmpl = spriteManager.GetSpriteTemplate(templateName);
            if(tmpl == nullptr) return;
            for(int screen :{0,1}){
                if(tmpl->IsLoadedInVRAM(screen)){
                    CM_UnloadSpriteTemplateFromVRAM(screen, templateName);
                }
            }
            spriteManager.templates.remove(tmpl->identity);
            spriteManager.templateRegistry.erase(templateName);
        }
        

        void CM_AddAnimationToTemplate(const std::string& templateName, const std::string& animationName, const std::vector<int>& frames, int frameDelay) {
            if (spriteManager.templateRegistry.find(templateName) == spriteManager.templateRegistry.end()) {
                return;
            }
            spriteManager.GetSpriteTemplate(templateName)->AddAnimation(animationName, frames, frameDelay);
        }


        Identifier CM_CreateSpritePalette(const std::string& paletteName){
            SpritePalette* pltt = spriteManager.palettes.create(paletteName.c_str());
            if(pltt != nullptr){
                spriteManager.paletteRegistry[paletteName] = pltt->identity;
            }
            else{
                std::cout << "Could not create Sprite Palette: " << paletteName << ", Limit Exceeded" << std::endl;
            }
            return pltt->identity;

        }


        void CM_LoadSpritePaletteToVRAM(int screen, const std::string& paletteName){
            SpritePalette* pltt = spriteManager.GetSpritePalette(paletteName);
            if(pltt == nullptr) return;
            if (pltt->IsLoadedInVRAM(screen)) {
                std::cout << paletteName << " already in VRAM on screen " << screen << "\n";
                return;
            }
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(screen);
            Identifier* vramId = registry.palettesInVRAM.create();
            if (vramId != nullptr) {
                pltt->LoadPaletteToVRAM(screen, *vramId);
            } else {
                std::cout << "VRAM Template Pool Full! Cannot load " << paletteName << "\n";
            }
        }


        void CM_UnloadSpritePaletteFromVRAM(int screen, const std::string& paletteName){
            SpritePalette* pltt = spriteManager.GetSpritePalette(paletteName);
            if(pltt == nullptr) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(screen);
            registry.palettesInVRAM.remove(pltt->GetPaletteVRAM(screen));
            pltt->UnloadPaletteFromVRAM(screen);
        }


        void CM_DeleteSpritePalette(const std::string& paletteName){
            SpritePalette* pltt = spriteManager.GetSpritePalette(paletteName);
            if(pltt == nullptr) return;
            for(int screen :{0,1}){
                if(pltt->IsLoadedInVRAM(screen)){
                    CM_UnloadSpritePaletteFromVRAM(screen, paletteName);
                }
            }
            spriteManager.palettes.remove(pltt->identity);
            spriteManager.paletteRegistry.erase(paletteName);
        }


        Identifier CM_CreateSprite(int screen, const std::string& templateName, const std::string& paletteName, Pos pos) {
            if (spriteManager.templateRegistry.find(templateName) == spriteManager.templateRegistry.end()) {
                std::cout << "Sprite Template " << templateName <<" doesn't exist" << std::endl;
                return {};  
            }
            if(spriteManager.paletteRegistry.find(paletteName) == spriteManager.paletteRegistry.end()){
                std::cout << "Sprite Palette " << paletteName <<" doesn't exist" << std::endl;
                return {};  
            }
            SpriteTemplate* spritetemplate = spriteManager.GetSpriteTemplate(templateName);
            SpritePalette* spritepalette = spriteManager.GetSpritePalette(paletteName);
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(screen);
            SpriteInstance* sprite = registry.sprites.create(screen, spritetemplate, spritepalette, pos);
            if(sprite == nullptr){
                std::cout << "Exceeded Sprite Limit On Screen " << screen << std::endl;
                return {};
            }
            return sprite->identity;
        }


        Identifier CM_CreateSprite(int screen, const std::string& templateName, Pos pos) {
            if (spriteManager.templateRegistry.find(templateName) == spriteManager.templateRegistry.end()) {
                std::cout << "Sprite Template " << templateName <<" doesn't exist" << std::endl;
                return {};  
            }
            if(spriteManager.paletteRegistry.begin() == spriteManager.paletteRegistry.end()){
                std::cout << "Default Sprite Palette doesn't exist" << std::endl;
                return {};  
            }
            SpriteTemplate* spritetemplate = spriteManager.GetSpriteTemplate(templateName);
            SpritePalette* spritepalette = spriteManager.GetSpritePalette(spriteManager.paletteRegistry.begin()->first);
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(screen);
            SpriteInstance* sprite = registry.sprites.create(screen, spritetemplate, spritepalette, pos);
            if(sprite == nullptr){
                std::cout << "Exceeded Sprite Limit On Screen " << screen << std::endl;
                return {};
            }
            return sprite->identity;
        }

        bool CM_isSpriteValid(Identifier spriteIdentity){
            SpriteInstance* sprite = spriteManager.GetSpriteInstance(spriteIdentity);
            if(sprite == nullptr) return false;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(sprite->screen);
            return registry.sprites.isValid(sprite->identity);
        }


        void CM_UpdateSprites() {
            spriteManager.screen0.sprites.forEach([](SpriteInstance& sprite) {sprite.Update();});
            spriteManager.screen1.sprites.forEach([](SpriteInstance& sprite) {sprite.Update();});
        }
        

        void CM_DestroyAllSprites() {
            for(ScreenRegistry* registry : {&spriteManager.screen0, &spriteManager.screen1}){
                registry->sprites.clear();
            }
            NF_SpriteOamSet(ScreenConstants::bottomScreen);
            NF_SpriteOamSet(ScreenConstants::topScreen);
            swiWaitForVBlank();
            oamUpdate(&oamMain);
            oamUpdate(&oamSub);
        }


        void CM_DestroySprite(Identifier spriteIdentity){
            if(!CM_isSpriteValid(spriteIdentity)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(spriteIdentity.screen.value());
            registry.sprites.remove(spriteIdentity);
        }


        void CM_MoveSprite(Identifier spriteIdentity, Pos position){
            if(!CM_isSpriteValid(spriteIdentity)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(spriteIdentity.screen.value());
            registry.sprites.get(spriteIdentity)->Move(position);
        }


        void CM_SetSpriteAnimation(Identifier spriteIdentity, const std::string& animationName){
            if(!CM_isSpriteValid(spriteIdentity)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(spriteIdentity.screen.value());
            registry.sprites.get(spriteIdentity)->SetAnimation(animationName);
        }


        void CM_SetSpriteLayer(Identifier spriteIdentity, int layer){
            if(!CM_isSpriteValid(spriteIdentity)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(spriteIdentity.screen.value());
            registry.sprites.get(spriteIdentity)->SetLayer(layer);
        }


        void CM_SetSpriteVisibility(Identifier spriteIdentity, bool isVisible){
            if(!CM_isSpriteValid(spriteIdentity)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(spriteIdentity.screen.value());
            registry.sprites.get(spriteIdentity)->Show(isVisible);
        }


        void CM_FlipSpriteHorizontaly(Identifier spriteIdentity, bool isFlipped){
            if(!CM_isSpriteValid(spriteIdentity)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(spriteIdentity.screen.value());
            registry.sprites.get(spriteIdentity)->HFlip(isFlipped);
        }


        bool CM_GetSpriteHorizontalFlip(Identifier sprite){
            if(!CM_isSpriteValid(sprite)) return false;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(sprite.screen.value());
            return registry.sprites.get(sprite)->GetHFlip();
        }


        void CM_FlipSpriteVerticaly(Identifier sprite, bool isFlipped){
            if(!CM_isSpriteValid(sprite)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(sprite.screen.value());
            registry.sprites.get(sprite)->VFlip(isFlipped);
        }


        bool CM_GetSpriteVerticalFlip(Identifier sprite){
            if(!CM_isSpriteValid(sprite)) return false;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(sprite.screen.value());
            return registry.sprites.get(sprite)->GetVFlip();
        }


        void CM_SetSpriteAnimationFrame(Identifier sprite, int frame){
            if(!CM_isSpriteValid(sprite)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(sprite.screen.value());
            registry.sprites.get(sprite)->SetAnimationFrame(frame);
        }


        Identifier CM_CreateRotScaleGroup(int screen, float rotation, float scaleX, float scaleY){
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(screen);
            RotScaleGroup* group = registry.rotScaleGroups.create(screen, rotation, scaleX, scaleY);
            if(group == nullptr){
                std::cout << "Exceeded Group Limit On Screen " << screen << std::endl;
                return {};
            }
            group->ApplyRotationAndScale();
            return group->identity;
        }


        void CM_BindSpriteToRotScaleGroup(Identifier sprite, Identifier group, bool doubleSize){
            if(!CM_isSpriteValid(sprite)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(sprite.screen.value());
            registry.sprites.get(sprite)->BindToRotScaleGroup(group.id, doubleSize);
            registry.rotScaleGroups.get(group)->AddSpritetoGroup(sprite);
        }
        
        
        void CM_RemoveSpriteFromRotScaleGroup(Identifier sprite, Identifier group){
            if(!CM_isSpriteValid(sprite)) return;
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(group.screen.value());
            registry.sprites.get(sprite)->RemoveFromRotScaleGroup();
            registry.rotScaleGroups.get(group)->RemoveSpriteFromGroup(sprite);
        }


        void CM_RotateGroup(Identifier group, float rotation){
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(group.screen.value());
            RotScaleGroup& localGroup = *registry.rotScaleGroups.get(group);
            localGroup.rotation = rotation;
            localGroup.ApplyRotationAndScale();
        }


        void CM_ScaleGroup(Identifier group, float scaleX, float scaleY){
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(group.screen.value());
            RotScaleGroup& localGroup = *registry.rotScaleGroups.get(group);
            localGroup.scaleX = scaleX;
            localGroup.scaleY = scaleY;
            localGroup.ApplyRotationAndScale();
        }


        void CM_RotateAndScaleGroup(Identifier group, float rotation, float scaleX, float scaleY){
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(group.screen.value());
            RotScaleGroup& localGroup = *registry.rotScaleGroups.get(group);
            localGroup.rotation = rotation;
            localGroup.scaleX = scaleX;
            localGroup.scaleY = scaleY;
            localGroup.ApplyRotationAndScale();
        }


        void CM_RemoveAllSpritesFromRotScaleGroup(Identifier group){
            ScreenRegistry& registry = spriteManager.GetScreenRegistry(group.screen.value());
            RotScaleGroup& localGroup = *registry.rotScaleGroups.get(group);
            for(auto& sprite: localGroup.spritesInGroup){
                CM_RemoveSpriteFromRotScaleGroup(sprite, localGroup.identity);
            }
            localGroup.spritesInGroup.clear();
        }