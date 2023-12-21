// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"

void UMapEditor::ResizeEvent(FViewport* ViewPort, uint32 val)
{
    //Эвент изменения размера экрана вызывается только если размер окна действительно имзменился
    if (OldViewPortSize != ViewPort->GetSizeXY()) {
        OldViewPortSize = ViewPort->GetSizeXY();

        OnResize(OldViewPortSize);
    }
}

UMapEditor::UMapEditor(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
    //При создании редактора карты связывается встроеный ViewportResizedEvent и кастомный ResizeEvent, доступный для блюпринтов
    GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &UMapEditor::ResizeEvent);
}
