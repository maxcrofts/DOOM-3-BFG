/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").  

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#pragma hdrstop
#include "../precompiled.h"
#include "Simd_Generic.h"

//===============================================================
//
//	Generic implementation of idSIMDProcessor
//
//===============================================================


/*
============
idSIMD_Generic::GetName
============
*/
const char * idSIMD_Generic::GetName() const {
	return "generic code";
}

/*
============
idSIMD_Generic::MinMax
============
*/
void VPCALL idSIMD_Generic::MinMax( float &min, float &max, const float *src, const int count ) {
	min = idMath::INFINITY;
	max = -idMath::INFINITY;
	for ( int i = 0; i < count; i++ ) {
		if ( src[i] < min ) {
			min = src[i];
		}
		if ( src[i] > max ) {
			max = src[i];
		}
	}
}

/*
============
idSIMD_Generic::MinMax
============
*/
void VPCALL idSIMD_Generic::MinMax( idVec2 &min, idVec2 &max, const idVec2 *src, const int count ) {
	min[0] = min[1] = idMath::INFINITY;
	max[0] = max[1] = -idMath::INFINITY;
	for ( int i = 0; i < count; i++ ) {
		const idVec2 &v = src[i];
		if ( v[0] < min[0] ) {
			min[0] = v[0];
		}
		if ( v[0] > max[0] ) {
			max[0] = v[0];
		}
		if ( v[1] < min[1] ) {
			min[1] = v[1];
		}
		if ( v[1] > max[1] ) {
			max[1] = v[1];
		}
	}
}

/*
============
idSIMD_Generic::MinMax
============
*/
void VPCALL idSIMD_Generic::MinMax( idVec3 &min, idVec3 &max, const idVec3 *src, const int count ) {
	min[0] = min[1] = min[2] = idMath::INFINITY;
	max[0] = max[1] = max[2] = -idMath::INFINITY;
	for ( int i = 0; i < count; i++ ) {
		const idVec3 &v = src[i];
		if ( v[0] < min[0] ) {
			min[0] = v[0];
		}
		if ( v[0] > max[0] ) {
			max[0] = v[0];
		}
		if ( v[1] < min[1] ) {
			min[1] = v[1];
		}
		if ( v[1] > max[1] ) {
			max[1] = v[1];
		}
		if ( v[2] < min[2] ) {
			min[2] = v[2];
		}
		if ( v[2] > max[2] ) {
			max[2] = v[2];
		}
	}
}

/*
============
idSIMD_Generic::MinMax
============
*/
void VPCALL idSIMD_Generic::MinMax( idVec3 &min, idVec3 &max, const idDrawVert *src, const int count ) {
	min[0] = min[1] = min[2] = idMath::INFINITY;
	max[0] = max[1] = max[2] = -idMath::INFINITY;
	for ( int i = 0; i < count; i++ ) {
		const idVec3 &v = src[i].xyz;
		if ( v[0] < min[0] ) {
			min[0] = v[0];
		}
		if ( v[0] > max[0] ) {
			max[0] = v[0];
		}
		if ( v[1] < min[1] ) {
			min[1] = v[1];
		}
		if ( v[1] > max[1] ) {
			max[1] = v[1];
		}
		if ( v[2] < min[2] ) {
			min[2] = v[2];
		}
		if ( v[2] > max[2] ) {
			max[2] = v[2];
		}
	}
}

/*
============
idSIMD_Generic::MinMax
============
*/
void VPCALL idSIMD_Generic::MinMax( idVec3 &min, idVec3 &max, const idDrawVert *src, const triIndex_t *indexes, const int count ) {
	min[0] = min[1] = min[2] = idMath::INFINITY;
	max[0] = max[1] = max[2] = -idMath::INFINITY;
	for ( int i = 0; i < count; i++ ) {
		const idVec3 &v = src[indexes[i]].xyz;
		if ( v[0] < min[0] ) {
			min[0] = v[0];
		}
		if ( v[0] > max[0] ) {
			max[0] = v[0];
		}
		if ( v[1] < min[1] ) {
			min[1] = v[1];
		}
		if ( v[1] > max[1] ) {
			max[1] = v[1];
		}
		if ( v[2] < min[2] ) {
			min[2] = v[2];
		}
		if ( v[2] > max[2] ) {
			max[2] = v[2];
		}
	}
}

/*
============
idSIMD_Generic::BlendJoints
============
*/
void VPCALL idSIMD_Generic::BlendJoints( idJointQuat *joints, const idJointQuat *blendJoints, const float lerp, const int *index, const int numJoints ) {
	for ( int i = 0; i < numJoints; i++ ) {
		int j = index[i];
		joints[j].q.Slerp( joints[j].q, blendJoints[j].q, lerp );
		joints[j].t.Lerp( joints[j].t, blendJoints[j].t, lerp );
		joints[j].w = 0.0f;
	}
}

/*
============
idSIMD_Generic::BlendJointsFast
============
*/
void VPCALL idSIMD_Generic::BlendJointsFast( idJointQuat *joints, const idJointQuat *blendJoints, const float lerp, const int *index, const int numJoints ) {
	for ( int i = 0; i < numJoints; i++ ) {
		int j = index[i];
		joints[j].q.Lerp( joints[j].q, blendJoints[j].q, lerp );
		joints[j].t.Lerp( joints[j].t, blendJoints[j].t, lerp );
		joints[j].w = 0.0f;
	}
}

/*
============
idSIMD_Generic::ConvertJointQuatsToJointMats
============
*/
void VPCALL idSIMD_Generic::ConvertJointQuatsToJointMats( idJointMat *jointMats, const idJointQuat *jointQuats, const int numJoints ) {
	for ( int i = 0; i < numJoints; i++ ) {
		jointMats[i].SetRotation( jointQuats[i].q.ToMat3() );
		jointMats[i].SetTranslation( jointQuats[i].t );
	}
}

/*
============
idSIMD_Generic::ConvertJointMatsToJointQuats
============
*/
void VPCALL idSIMD_Generic::ConvertJointMatsToJointQuats( idJointQuat *jointQuats, const idJointMat *jointMats, const int numJoints ) {
	for ( int i = 0; i < numJoints; i++ ) {
		jointQuats[i] = jointMats[i].ToJointQuat();
	}
}

/*
============
idSIMD_Generic::TransformJoints
============
*/
void VPCALL idSIMD_Generic::TransformJoints( idJointMat *jointMats, const int *parents, const int firstJoint, const int lastJoint ) {
	for ( int i = firstJoint; i <= lastJoint; i++ ) {
		assert( parents[i] < i );
		jointMats[i] *= jointMats[parents[i]];
	}
}

/*
============
idSIMD_Generic::UntransformJoints
============
*/
void VPCALL idSIMD_Generic::UntransformJoints( idJointMat *jointMats, const int *parents, const int firstJoint, const int lastJoint ) {
	for ( int i = lastJoint; i >= firstJoint; i-- ) {
		assert( parents[i] < i );
		jointMats[i] /= jointMats[parents[i]];
	}
}
