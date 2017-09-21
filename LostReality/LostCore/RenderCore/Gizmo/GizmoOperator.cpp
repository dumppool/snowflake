#include "stdafx.h"

LostCore::FGizmoOperator::FGizmoOperator() 
	: Target(nullptr)
	, LastDragEnd(FFloat3::GetInfinity())
	, ActivedComponent(nullptr)
{
	World.SetIdentity();
	Components.clear();
}

LostCore::FGizmoOperator::~FGizmoOperator()
{
	Fini();
}

bool LostCore::FGizmoOperator::Load(const string & url)
{
	FJson config;
	if (FDirectoryHelper::Get()->GetGizmoJson(url, config))
	{
		return Config(config);
	}

	return false;
}

bool LostCore::FGizmoOperator::Config(const FJson & config)
{
	if (config.find(K_PLACER) != config.end())
	{
		FJson modelConfig;
		auto model = FModelFactory::NewModel(config[K_PLACER]);
		if (model != nullptr)
		{
			FComp comp;
			FFloat4x4 w;

			// PlacerX
			comp.Id = EOp::PlacerX;
			comp.Model = model;
			comp.Model->EnableDepthTest(false);
			comp.Model->SetColor(FColor128((uint32)0xffff0000));
			comp.Local = w.SetRotate(FFloat3(0.0f, 90.0f, 0.0f));
			comp.Plane = FPlane(FFloat3(0.0f, 1.0f, 0.0f), 0.0f);
			Components.push_back(comp);

			// PlacerY
			comp.Id = EOp::PlacerY;
			comp.Model = FModelFactory::NewModel(config[K_PLACER]);
			comp.Model->EnableDepthTest(false);
			comp.Model->SetColor(FColor128((uint32)0xff00ff00));
			comp.Local = w.SetRotate(FFloat3(90.0f, 0.0f, 0.0f));
			comp.Plane = FPlane(FFloat3(0.0f, 0.0f, 1.0f), 0.0f);
			Components.push_back(comp);

			// PlacerZ
			comp.Id = EOp::PlacerZ;
			comp.Model = FModelFactory::NewModel(config[K_PLACER]);
			comp.Model->EnableDepthTest(false);
			comp.Model->SetColor(FColor128((uint32)0xff0000ff));
			comp.Local = w.SetRotate(FFloat3(0.0f, 0.0f, 0.0f));
			comp.Plane = FPlane(FFloat3(1.0f, 0.0f, 0.0f), 0.0f);
			Components.push_back(comp);

			// TODO: Rotator...

			return true;

		}
	}

	return false;
}

bool LostCore::FGizmoOperator::RayTest(FRay & ray, bool active)
{
	if (Target == nullptr)
	{
		return false;
	}

	float minDist = ray.Distance;
	ActivedComponent = nullptr;

	auto travel = [&](FComp& comp)
	{
		auto m = comp.Model;
		if (m == nullptr || m->GetBoundingBox() == nullptr)
		{
			return;
		}

		m->GetBoundingBox()->bVisible = false;
		if (m->RayTest(ray, minDist))
		//if (RayBoxIntersect(ray, *m->GetBoundingBox(), m->GetWorldMatrix().Invert(), minDist))
		{
			ActivedComponent = &comp;
			ray.Distance = minDist;
		}
	};

	for_each(Components.begin(), Components.end(), travel);
	if (ActivedComponent != nullptr)
	{
		if (active)
		{
			// TODO: Need a highlight method.
			ActivedComponent->Model->GetBoundingBox()->bVisible = true;
		}
		else
		{
			ActivedComponent = nullptr;
		}

		return true;
	}

	return false;
}

void LostCore::FGizmoOperator::OnDragging(int32 x, int32 y, FBasicCamera* camera)
{
	if (ActivedComponent == nullptr || Target == nullptr || camera == nullptr)
	{
		return;
	}

	auto vec = camera->ScreenToWorld(x, y, 1.0f);
	if (LastDragEnd.IsInfinity())
	{
		LastDragEnd = vec;
		return;
	}

	// TODO: 方法很有问题,暂时将就下着用吧,有空再看看UE4怎么做的
	auto delta = vec - LastDragEnd;
	LastDragEnd = vec;
	auto world = ActivedComponent->Model->GetWorldMatrix();
	auto dist = delta.Dot(world.GetForwardVector());
	auto wv = world * camera->GetViewMatrix();
	dist = dist * abs(wv.GetOrigin().Z);
	Target->SetWorldMatrix(world.AddTranslate(world.GetForwardVector() * dist));
}

void LostCore::FGizmoOperator::EndDrag()
{
	LastDragEnd = FFloat3::GetInfinity();
}

void LostCore::FGizmoOperator::Tick()
{
	if (Target == nullptr)
	{
		World.SetIdentity();
		return;
	}

	World = Target->GetWorldMatrix();
	for (auto& comp : Components)
	{
		if (comp.Model != nullptr)
		{
			auto world = comp.Local * World;
			comp.Model->SetWorldMatrix(world);
			comp.Model->Tick();
		}
	}
}

void LostCore::FGizmoOperator::Draw()
{
	if (Target == nullptr)
	{
		return;
	}

	for (auto& comp : Components)
	{
		if (comp.Model != nullptr)
		{
			comp.Model->Draw();
		}
	}
}

void LostCore::FGizmoOperator::SetTarget(FBasicModel * target)
{
	Target = target;
}

LostCore::FBasicModel * LostCore::FGizmoOperator::GetTarget()
{
	return Target;
}

void LostCore::FGizmoOperator::Fini()
{
	for (auto& comp : Components)
	{
		SAFE_DELETE(comp.Model);
	}

	Components.clear();
	Target = nullptr;
}

LostCore::FGizmoOperator::FComp::FComp()
	: Model(nullptr)
{
	Reset();
}

void LostCore::FGizmoOperator::FComp::Reset()
{
	Id = EOp::Null;
	if (Model != nullptr)
	{
		Model->GetBoundingBox()->bVisible = false;
		Model = nullptr;
	}

	Local.SetIdentity();
}

bool LostCore::FGizmoOperator::FComp::operator==(const FComp& rhs) const
{
	return Id == rhs.Id;
}
