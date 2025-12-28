#pragma once

#include "drawDebugHelpers.h"
#define draw_sphere(location) if(GetWorld()) DrawDebugSphere(GetWorld(), location, 25.f, 12, FColor::Red, true);
#define draw_sphere_single(location) if(GetWorld()) DrawDebugSphere(GetWorld(), location, 25.f, 12, FColor::Red, false, -1.f);
#define draw_line(start_location, end_location) if(GetWorld()) DrawDebugLine(GetWorld(), start_location, end_location, FColor::Red, true, -1.f, 0, 1.f);
#define draw_line_single(start_location, end_location) if(GetWorld()) DrawDebugLine(GetWorld(), start_location, end_location, FColor::Red, false, -1.f, 0, 1.f);
#define draw_point(location) if(GetWorld())	DrawDebugPoint(GetWorld(), location, 15.f, FColor::Red, true);
#define draw_point_single(location) if(GetWorld())	DrawDebugPoint(GetWorld(), location, 15.f, FColor::Red, false, -1.f);
#define draw_vector(start_location, end_location) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), start_location, end_location, FColor::Red, true, -1.f, 0, 1.f); \
        DrawDebugPoint(GetWorld(), end_location, 15.f, FColor::Red, true); \
	}
#define draw_vector_single(start_location, end_location) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), start_location, end_location, FColor::Red, false, -1.f, 0, 1.f); \
        DrawDebugPoint(GetWorld(), end_location, 15.f, FColor::Red, false, -1.f); \
	}