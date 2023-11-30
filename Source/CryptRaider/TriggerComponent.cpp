// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"
// Sets default values for this component's properties
UTriggerComponent::UTriggerComponent()
{
    UE_LOG(LogTemp, Display, TEXT("TriggerComp Constructor"));
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UTriggerComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Display, TEXT("Begin Play TriggerComp"));

}

AActor *UTriggerComponent::GetAcceptableActor() const
{
    TArray<AActor*> Actors; 
    GetOverlappingActors(Actors);

    // Get all actors
    for (AActor* Actor: Actors) 
    {
        if(Actor->ActorHasTag(TriggerActorTag) && !Actor->ActorHasTag("Grabbed")) return Actor;
    }

    return nullptr;
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if(Mover == nullptr) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Mover is null"));
        return;
    }

    AActor* AcceptedActor = GetAcceptableActor();

    if(AcceptedActor == nullptr) 
    {
        // Relocking
        Mover->SetShouldMove(false);

    } else 
    {

        UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(AcceptedActor->GetRootComponent());

        if(Component == nullptr) return;
        // Disable physics
        Component->SetSimulatePhysics(false);
        // Attach the actor to the wall
        AcceptedActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
        // Unlocking
        Mover->SetShouldMove(true);
    }
   
}

void UTriggerComponent::SetMover(UMover *NewMover)
{
    Mover = NewMover;
}
