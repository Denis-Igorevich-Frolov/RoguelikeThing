// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"

UMapTile::UMapTile(const FObjectInitializer& Object) : UUserWidget(Object)
{
    CellsCoordWrapper = NewObject<UCoordWrapperOfTable>();
}
