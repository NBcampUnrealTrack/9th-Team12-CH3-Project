// AN_PlaySoundAtMeshSocket.cpp

#include "Animation/Notifies/AN_PlaySoundAtMeshSocket.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UAN_PlaySoundAtMeshSocket::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !Sound)
	{
		return;
	}

	const FVector Location =
		SocketName != NAME_None && MeshComp->DoesSocketExist(SocketName)
			? MeshComp->GetSocketLocation(SocketName)
			: MeshComp->GetComponentLocation();

	UGameplayStatics::PlaySoundAtLocation(
		MeshComp,
		Sound,
		Location,
		VolumeMultiplier,
		PitchMultiplier
	);
}