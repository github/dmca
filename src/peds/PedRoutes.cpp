#include "common.h"

#include "main.h"
#include "PedRoutes.h"

CRouteNode gaRoutes[NUMPEDROUTES];

void
CRouteNode::Initialise()
{
	for (int i = 0; i < NUMPEDROUTES; i++) {
		gaRoutes[i].m_route = -1;
		gaRoutes[i].m_pos = CVector(0.0f, 0.0f, 0.0f);
	}
}

int16
CRouteNode::GetRouteThisPointIsOn(int16 point)
{
	return gaRoutes[point].m_route;
}

// Actually GetFirstPointOfRoute
int16
CRouteNode::GetRouteStart(int16 route)
{
	for (int i = 0; i < NUMPEDROUTES; i++) {
		if (route == gaRoutes[i].m_route)
			return i;
	}
	return -1;
}

CVector
CRouteNode::GetPointPosition(int16 point)
{
	return gaRoutes[point].m_pos;
}

void
CRouteNode::AddRoutePoint(int16 route, CVector pos)
{
	uint16 point;
	for (point = 0; point < NUMPEDROUTES; point++) {
		if (gaRoutes[point].m_route == -1)
			break;
	}
#ifdef FIX_BUGS
	if (point == NUMPEDROUTES)
		return;
#endif
	gaRoutes[point].m_route = route;
	gaRoutes[point].m_pos = pos;
}

void
CRouteNode::RemoveRoute(int16 route)
{
	uint16 first_point, last_point, i;
	for (first_point = 0; first_point < NUMPEDROUTES; first_point++) {
		if (gaRoutes[first_point].m_route == route)
			break;
	}
	if (first_point == NUMPEDROUTES)
		return;
	for (last_point = first_point; last_point < NUMPEDROUTES; last_point++)
		if (gaRoutes[last_point].m_route != route)
			break;
	uint16 diff = last_point - first_point;
#ifdef FIX_BUGS	
	for (i = first_point; i < NUMPEDROUTES - diff; i++)
		gaRoutes[i] = gaRoutes[i + diff];
#else
	for (i = 0; i < diff; i++)
		gaRoutes[first_point + i] = gaRoutes[last_point + i];
#endif
	for (i = NUMPEDROUTES - diff; i < NUMPEDROUTES; i++)
		gaRoutes[i].m_route = -1;
}
