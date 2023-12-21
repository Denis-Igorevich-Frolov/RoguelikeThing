// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"

void UMapEditor::ResizeEvent(FViewport* ViewPort, uint32 val)
{
    //����� ��������� ������� ������ ���������� ������ ���� ������ ���� ������������� ����������
    if (OldViewPortSize != ViewPort->GetSizeXY()) {
        OldViewPortSize = ViewPort->GetSizeXY();

        OnResize(OldViewPortSize);
    }
}

UMapEditor::UMapEditor(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
    //��� �������� ��������� ����� ����������� ��������� ViewportResizedEvent � ��������� ResizeEvent, ��������� ��� ����������
    GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &UMapEditor::ResizeEvent);
}
