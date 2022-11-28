// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TEST.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/MainCharacter.h"
#include "AIController.h"


UBTTask_TEST::UBTTask_TEST()
{

}

void UBTTask_TEST::OnGameplayTaskActivated(UGameplayTask& task)
{
	Super::OnGameplayTaskActivated(task);
}

EBTNodeResult::Type UBTTask_TEST::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* controller = OwnerComp.GetAIOwner();
	AActor* owner = controller->GetCharacter();

	if (!owner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("executeTask Failed"));
		return EBTNodeResult::Type::Failed;
	}
	UNavigationSystemV1* navSystem =  UNavigationSystemV1::GetCurrent(GetWorld());
	if (navSystem)
	{
		FNavLocation navLoc;
		navSystem->GetRandomPointInNavigableRadius(owner->GetActorLocation(), 2000.f, navLoc);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetLoc", navLoc);
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("executeTask"));

	return EBTNodeResult::Type::Succeeded;
}
