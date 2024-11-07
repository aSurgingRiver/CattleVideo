// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideoControl.h"
//#include "CattleVideoControlImp.h"
#include "Slate/SlateTextures.h"


//UCattleVideoControl::UCattleVideoControl(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//	//, Texture(nullptr)
//{
//}
//

UCattleVideoControl::UCattleVideoControl() {

}

UCattleVideoControl::~UCattleVideoControl() {

}
UTexture2D* UCattleVideoControl::GetTexture2D() {
	return nullptr;
}

void UCattleVideoControl::Close() {

}
bool UCattleVideoControl::Open(FString URL, FCattleVideoOption& option) {
	return false;
}

void UCattleVideoControl::SetSound(TSharedPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> SoundGenerator) {

}
void UCattleVideoControl::Pause() {

}
void UCattleVideoControl::Resume() {

}
void UCattleVideoControl::Speed(uint32) {

}
void UCattleVideoControl::AddSize(std::shared_ptr<cattlevideo::VideoSize> size) {

}
void UCattleVideoControl::DelSize(std::shared_ptr<cattlevideo::VideoSize> size) {

}
int  UCattleVideoControl::Duration() {
	return 0;
}
bool UCattleVideoControl::Seek(int pos) {
	return false;
}
void UCattleVideoControl::ProgressSynchro(bool synchro) {
	
}