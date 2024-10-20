// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Enumerations/MapEditorBrushType.h"

//Функция, возвращающая соответствующий текущему стилю кисти стиль клетки
FCellType UMapEditorBrushType::getCellType()
{
	switch (BrushType)
	{
	case FMapEditorBrushType::Emptiness:
		return FCellType::Emptiness;
		break;
	case FMapEditorBrushType::Error:
		return FCellType::Error;
		break;
	case FMapEditorBrushType::Corridor:
		return FCellType::Corridor;
		break;
	case FMapEditorBrushType::Room:
		return FCellType::Room;
		break;
	default:
		return FCellType::Emptiness;
		break;
	}
}
