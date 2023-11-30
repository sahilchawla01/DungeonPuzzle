// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if(PhysicsHandle == nullptr) UE_LOG(LogTemp, Warning, TEXT("Physics Handle doesn't exist "));
	
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Set and check if physics handle exists, if not, return
	SetPhysicsHandle();
	if(PhysicsHandle == nullptr) return;

	// Check if grabbed component exists, if not, return
	if(PhysicsHandle->GetGrabbedComponent() == nullptr) return;

	// Set the location of the grabbed component
	FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
}

void UGrabber::Release() 
{
	UE_LOG(LogTemp, Display, TEXT("Released object"));

	// Set and check if physics handle exists, if not, return
	SetPhysicsHandle();
	if(PhysicsHandle == nullptr) return;

	// Check if grabbed component exists, if not, return
	if(PhysicsHandle->GetGrabbedComponent() == nullptr) return;

	// Remove Grabbed tag
	PhysicsHandle->GetGrabbedComponent()->GetOwner()->Tags.Remove("Grabbed");

	// Wake the physics component in case it sleeps
	PhysicsHandle->GetGrabbedComponent()->WakeAllRigidBodies();
	// Release the currently held component
	PhysicsHandle->ReleaseComponent();
}

void UGrabber::Grab()
{
	
	SetPhysicsHandle();

	if(PhysicsHandle == nullptr) return;

	FHitResult SweepResult;
	if(GetGrabbableInReach(SweepResult)) 
	{
		UE_LOG(LogTemp, Display, TEXT("Hit Actor is %s"), *(SweepResult.GetActor()->GetName()));

		

		// Get component and wake physics
		UPrimitiveComponent* HitComponent = SweepResult.GetComponent();
		HitComponent->SetSimulatePhysics(true);
		HitComponent->WakeAllRigidBodies();

		AActor* HitActor = SweepResult.GetActor();
		// Add Grabbed tag
		HitActor->Tags.Add("Grabbed");
		// Detach
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		// Grab the component
		PhysicsHandle->GrabComponentAtLocationWithRotation(HitComponent, NAME_None, SweepResult.ImpactPoint, GetComponentRotation());

		return;

	} else 
	{
		UE_LOG(LogTemp, Display, TEXT("Not Hit Anything"));
		return;
	}

}

void UGrabber::SetPhysicsHandle()
{
	// Check if not already set
	if(PhysicsHandle != nullptr) return;

	// If not set, set it
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	// If still doesn't exist, display a warning.
	if(PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Physics Handle doesn't exist even after trying to set it"));
		return;
	}
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const
{
	UWorld* World = GetWorld();
	// Raycast
	FVector Start = GetComponentLocation();
	FVector End = Start + (GetForwardVector() * MaxGrabDistance);


	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(GrabRadius);

	return World->SweepSingleByChannel(OutHitResult , Start, End, FQuat::Identity, ECC_GameTraceChannel2, CollisionShape);
}

