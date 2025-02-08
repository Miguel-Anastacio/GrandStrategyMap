#if WITH_EDITOR
#include "Factories/TextureFromBufferFactory.h"
UTexture2DFromBufferFactory::UTexture2DFromBufferFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass		= UTexture2D::StaticClass();
	bCreateNew			= true;
	bEditAfterNew		= true;
	Width = 256;
	Height = 256;
}

bool UTexture2DFromBufferFactory::ShouldShowInNewMenu() const
{
	// You may not create texture2d assets in the content browser
	return false;
}

UObject* UTexture2DFromBufferFactory::FactoryCreateNew( UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn )
{
	// Do not create a texture with bad dimensions.
	if(Width == 0 || Height == 0)
	{
		return nullptr;
	}
	UTexture2D* Object = NewObject<UTexture2D>(InParent, InClass, InName, Flags);
	
	// Set texture properties
	Object->Source.Init2DWithMipChain(Width, Height, TSF_BGRA8);

	if( Object->Source.IsValid() )
	{
		// Lock the mipmap and write the buffer data
		{
			FTextureSource::FMipLock MipLock(FTextureSource::ELockState::ReadWrite,&Object->Source,0);

			if ( MipLock.IsValid() )
			{
				int32 BufferSize = Width * Height * 4; // BGRA8 format
				FMemory::Memcpy(MipLock.Image.RawData, Buffer, BufferSize);
			}
		}
		
		Object->PostEditChange();
	}
	return Object;
}
#endif