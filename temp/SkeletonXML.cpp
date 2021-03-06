#include "ImportPhysXDOM.h"
#include "PhysicsDOM.h"
#include "FastXml.h"
#include "StringHelper.h"
#include <unordered_map>
#include <string>
#include <vector>

#pragma warning(disable:4100)

namespace IMPORT_PHYSX_DOM
{
	// 170 unique element types
	enum ElementType {
		ET_ActorFlags,
		ET_Actors,
		ET_AngularDamping,
		ET_AngularVelocity,
		ET_BounceThreshold,
		ET_BreakForce,
		ET_CMassLocalPose,
		ET_ChildPose,
		ET_ColumnScale,
		ET_ConstraintFlags,
		ET_ContactDistance,
		ET_ContactOffset,
		ET_ContactReportThreshold,
		ET_ConvexEdgeThreshold,
		ET_ConvexMesh,
		ET_Damping,
		ET_DistanceJointFlags,
		ET_DominanceGroup,
		ET_Drive,
		ET_DriveForceLimit,
		ET_DriveGearRatio,
		ET_DrivePosition,
		ET_DriveType,
		ET_DriveVelocity,
		ET_DynamicFriction,
		ET_ExternalCompliance,
		ET_ExternalDriveIterations,
		ET_Flags,
		ET_ForceLimit,
		ET_Format,
		ET_FrictionCombineMode,
		ET_Geometry,
		ET_GlobalPose,
		ET_HalfExtents,
		ET_HalfHeight,
		ET_HeightField,
		ET_HeightFieldFlags,
		ET_HeightScale,
		ET_Id,
		ET_InternalCompliance,
		ET_InternalDriveIterations,
		ET_InvInertiaScale0,
		ET_InvInertiaScale1,
		ET_InvMassScale0,
		ET_InvMassScale1,
		ET_Joint,
		ET_Length,
		ET_Limit,
		ET_LimitCone,
		ET_LinearDamping,
		ET_LinearLimit,
		ET_LinearVelocity,
		ET_Links,
		ET_LocalPose,
		ET_Lower,
		ET_Mass,
		ET_MassSpaceInertiaTensor,
		ET_Materials,
		ET_MaxAngularVelocity,
		ET_MaxContactImpulse,
		ET_MaxDepenetrationVelocity,
		ET_MaxDistance,
		ET_MaxMargin,
		ET_MaxNbActors,
		ET_MaxProjectionIterations,
		ET_MinCCDAdvanceCoefficient,
		ET_MinDistance,
		ET_Motion,
		ET_Name,
		ET_NbColumns,
		ET_NbRows,
		ET_NumActors,
		ET_OwnerClient,
		ET_Parent,
		ET_ParentPose,
		ET_PhysX30Collection,
		ET_Points,
		ET_ProjectionAngularTolerance,
		ET_ProjectionLinearTolerance,
		ET_PxActorRef,
		ET_PxAggregate,
		ET_PxArticulation,
		ET_PxArticulationLink,
		ET_PxArticulationRef,
		ET_PxBVH33TriangleMesh,
		ET_PxBoxGeometry,
		ET_PxCapsuleGeometry,
		ET_PxConvexMesh,
		ET_PxConvexMeshGeometry,
		ET_PxD6Joint,
		ET_PxDistanceJoint,
		ET_PxFixedJoint,
		ET_PxHeightField,
		ET_PxHeightFieldGeometry,
		ET_PxMaterial,
		ET_PxMaterialRef,
		ET_PxPlaneGeometry,
		ET_PxPrismaticJoint,
		ET_PxRevoluteJoint,
		ET_PxRigidDynamic,
		ET_PxRigidStatic,
		ET_PxShape,
		ET_PxShapeRef,
		ET_PxSphereGeometry,
		ET_PxSphericalJoint,
		ET_PxTriangleMeshGeometry,
		ET_QueryFilterData,
		ET_Radius,
		ET_RestOffset,
		ET_Restitution,
		ET_RestitutionCombineMode,
		ET_Rotation,
		ET_RowScale,
		ET_Scale,
		ET_SelfCollision,
		ET_SeparationTolerance,
		ET_Shapes,
		ET_SimulationFilterData,
		ET_SleepThreshold,
		ET_SolverIterationCounts,
		ET_Speed,
		ET_SphericalJointFlags,
		ET_StabilizationThreshold,
		ET_StaticFriction,
		ET_Stiffness,
		ET_SwingLimit,
		ET_SwingLimitContactDistance,
		ET_SwingLimitEnabled,
		ET_TangentialDamping,
		ET_TangentialStiffness,
		ET_TargetOrientation,
		ET_TargetVelocity,
		ET_Thickness,
		ET_Tolerance,
		ET_TriangleMesh,
		ET_Triangles,
		ET_TwistLimit,
		ET_TwistLimitContactDistance,
		ET_TwistLimitEnabled,
		ET_UpVector,
		ET_Upper,
		ET_Value,
		ET_WakeCounter,
		ET_YAngle,
		ET_ZAngle,
		ET_actor0,
		ET_actor1,
		ET_angular,
		ET_eACTOR0,
		ET_eACTOR1,
		ET_eSLERP,
		ET_eSWING,
		ET_eSWING1,
		ET_eSWING2,
		ET_eTWIST,
		ET_eX,
		ET_eY,
		ET_eZ,
		ET_force,
		ET_linear,
		ET_lower,
		ET_materialIndices,
		ET_minPositionIters,
		ET_minVelocityIters,
		ET_points,
		ET_samples,
		ET_torque,
		ET_upper,
		ET_yLimit,
		ET_zLimit,
		ET_LAST
	};

	// 1 unique attribute types
	enum AttributeType {
		AT_version,
		AT_LAST
	};

	struct ElementStruct
	{
		ElementType 	mType;
		const char		*mName;
	};

	static ElementStruct gElements[ET_LAST] =
	{
		ET_ActorFlags, "ActorFlags",
		ET_Actors, "Actors",
		ET_AngularDamping, "AngularDamping",
		ET_AngularVelocity, "AngularVelocity",
		ET_BounceThreshold, "BounceThreshold",
		ET_BreakForce, "BreakForce",
		ET_CMassLocalPose, "CMassLocalPose",
		ET_ChildPose, "ChildPose",
		ET_ColumnScale, "ColumnScale",
		ET_ConstraintFlags, "ConstraintFlags",
		ET_ContactDistance, "ContactDistance",
		ET_ContactOffset, "ContactOffset",
		ET_ContactReportThreshold, "ContactReportThreshold",
		ET_ConvexEdgeThreshold, "ConvexEdgeThreshold",
		ET_ConvexMesh, "ConvexMesh",
		ET_Damping, "Damping",
		ET_DistanceJointFlags, "DistanceJointFlags",
		ET_DominanceGroup, "DominanceGroup",
		ET_Drive, "Drive",
		ET_DriveForceLimit, "DriveForceLimit",
		ET_DriveGearRatio, "DriveGearRatio",
		ET_DrivePosition, "DrivePosition",
		ET_DriveType, "DriveType",
		ET_DriveVelocity, "DriveVelocity",
		ET_DynamicFriction, "DynamicFriction",
		ET_ExternalCompliance, "ExternalCompliance",
		ET_ExternalDriveIterations, "ExternalDriveIterations",
		ET_Flags, "Flags",
		ET_ForceLimit, "ForceLimit",
		ET_Format, "Format",
		ET_FrictionCombineMode, "FrictionCombineMode",
		ET_Geometry, "Geometry",
		ET_GlobalPose, "GlobalPose",
		ET_HalfExtents, "HalfExtents",
		ET_HalfHeight, "HalfHeight",
		ET_HeightField, "HeightField",
		ET_HeightFieldFlags, "HeightFieldFlags",
		ET_HeightScale, "HeightScale",
		ET_Id, "Id",
		ET_InternalCompliance, "InternalCompliance",
		ET_InternalDriveIterations, "InternalDriveIterations",
		ET_InvInertiaScale0, "InvInertiaScale0",
		ET_InvInertiaScale1, "InvInertiaScale1",
		ET_InvMassScale0, "InvMassScale0",
		ET_InvMassScale1, "InvMassScale1",
		ET_Joint, "Joint",
		ET_Length, "Length",
		ET_Limit, "Limit",
		ET_LimitCone, "LimitCone",
		ET_LinearDamping, "LinearDamping",
		ET_LinearLimit, "LinearLimit",
		ET_LinearVelocity, "LinearVelocity",
		ET_Links, "Links",
		ET_LocalPose, "LocalPose",
		ET_Lower, "Lower",
		ET_Mass, "Mass",
		ET_MassSpaceInertiaTensor, "MassSpaceInertiaTensor",
		ET_Materials, "Materials",
		ET_MaxAngularVelocity, "MaxAngularVelocity",
		ET_MaxContactImpulse, "MaxContactImpulse",
		ET_MaxDepenetrationVelocity, "MaxDepenetrationVelocity",
		ET_MaxDistance, "MaxDistance",
		ET_MaxMargin, "MaxMargin",
		ET_MaxNbActors, "MaxNbActors",
		ET_MaxProjectionIterations, "MaxProjectionIterations",
		ET_MinCCDAdvanceCoefficient, "MinCCDAdvanceCoefficient",
		ET_MinDistance, "MinDistance",
		ET_Motion, "Motion",
		ET_Name, "Name",
		ET_NbColumns, "NbColumns",
		ET_NbRows, "NbRows",
		ET_NumActors, "NumActors",
		ET_OwnerClient, "OwnerClient",
		ET_Parent, "Parent",
		ET_ParentPose, "ParentPose",
		ET_PhysX30Collection, "PhysX30Collection",
		ET_Points, "Points",
		ET_ProjectionAngularTolerance, "ProjectionAngularTolerance",
		ET_ProjectionLinearTolerance, "ProjectionLinearTolerance",
		ET_PxActorRef, "PxActorRef",
		ET_PxAggregate, "PxAggregate",
		ET_PxArticulation, "PxArticulation",
		ET_PxArticulationLink, "PxArticulationLink",
		ET_PxArticulationRef, "PxArticulationRef",
		ET_PxBVH33TriangleMesh, "PxBVH33TriangleMesh",
		ET_PxBoxGeometry, "PxBoxGeometry",
		ET_PxCapsuleGeometry, "PxCapsuleGeometry",
		ET_PxConvexMesh, "PxConvexMesh",
		ET_PxConvexMeshGeometry, "PxConvexMeshGeometry",
		ET_PxD6Joint, "PxD6Joint",
		ET_PxDistanceJoint, "PxDistanceJoint",
		ET_PxFixedJoint, "PxFixedJoint",
		ET_PxHeightField, "PxHeightField",
		ET_PxHeightFieldGeometry, "PxHeightFieldGeometry",
		ET_PxMaterial, "PxMaterial",
		ET_PxMaterialRef, "PxMaterialRef",
		ET_PxPlaneGeometry, "PxPlaneGeometry",
		ET_PxPrismaticJoint, "PxPrismaticJoint",
		ET_PxRevoluteJoint, "PxRevoluteJoint",
		ET_PxRigidDynamic, "PxRigidDynamic",
		ET_PxRigidStatic, "PxRigidStatic",
		ET_PxShape, "PxShape",
		ET_PxShapeRef, "PxShapeRef",
		ET_PxSphereGeometry, "PxSphereGeometry",
		ET_PxSphericalJoint, "PxSphericalJoint",
		ET_PxTriangleMeshGeometry, "PxTriangleMeshGeometry",
		ET_QueryFilterData, "QueryFilterData",
		ET_Radius, "Radius",
		ET_RestOffset, "RestOffset",
		ET_Restitution, "Restitution",
		ET_RestitutionCombineMode, "RestitutionCombineMode",
		ET_Rotation, "Rotation",
		ET_RowScale, "RowScale",
		ET_Scale, "Scale",
		ET_SelfCollision, "SelfCollision",
		ET_SeparationTolerance, "SeparationTolerance",
		ET_Shapes, "Shapes",
		ET_SimulationFilterData, "SimulationFilterData",
		ET_SleepThreshold, "SleepThreshold",
		ET_SolverIterationCounts, "SolverIterationCounts",
		ET_Speed, "Speed",
		ET_SphericalJointFlags, "SphericalJointFlags",
		ET_StabilizationThreshold, "StabilizationThreshold",
		ET_StaticFriction, "StaticFriction",
		ET_Stiffness, "Stiffness",
		ET_SwingLimit, "SwingLimit",
		ET_SwingLimitContactDistance, "SwingLimitContactDistance",
		ET_SwingLimitEnabled, "SwingLimitEnabled",
		ET_TangentialDamping, "TangentialDamping",
		ET_TangentialStiffness, "TangentialStiffness",
		ET_TargetOrientation, "TargetOrientation",
		ET_TargetVelocity, "TargetVelocity",
		ET_Thickness, "Thickness",
		ET_Tolerance, "Tolerance",
		ET_TriangleMesh, "TriangleMesh",
		ET_Triangles, "Triangles",
		ET_TwistLimit, "TwistLimit",
		ET_TwistLimitContactDistance, "TwistLimitContactDistance",
		ET_TwistLimitEnabled, "TwistLimitEnabled",
		ET_UpVector, "UpVector",
		ET_Upper, "Upper",
		ET_Value, "Value",
		ET_WakeCounter, "WakeCounter",
		ET_YAngle, "YAngle",
		ET_ZAngle, "ZAngle",
		ET_actor0, "actor0",
		ET_actor1, "actor1",
		ET_angular, "angular",
		ET_eACTOR0, "eACTOR0",
		ET_eACTOR1, "eACTOR1",
		ET_eSLERP, "eSLERP",
		ET_eSWING, "eSWING",
		ET_eSWING1, "eSWING1",
		ET_eSWING2, "eSWING2",
		ET_eTWIST, "eTWIST",
		ET_eX, "eX",
		ET_eY, "eY",
		ET_eZ, "eZ",
		ET_force, "force",
		ET_linear, "linear",
		ET_lower, "lower",
		ET_materialIndices, "materialIndices",
		ET_minPositionIters, "minPositionIters",
		ET_minVelocityIters, "minVelocityIters",
		ET_points, "points",
		ET_samples, "samples",
		ET_torque, "torque",
		ET_upper, "upper",
		ET_yLimit, "yLimit",
		ET_zLimit, "zLimit",
	};

	struct AttributeStruct
	{
		AttributeType mType;
		const char	  *mName;
	};

	static AttributeStruct gAttributes[AT_LAST] =
	{
		AT_version, "version",
	};

	typedef std::unordered_map< std::string, ElementType > ElementTypeMap;
	typedef std::unordered_map< std::string, AttributeType > AttributeTypeMap;

	ElementTypeMap gElementsMap;
	AttributeTypeMap gAttributesMap;

	static void initMaps(void)
	{
		for (auto &i : gElements)
		{
			gElementsMap[std::string(i.mName)] = i.mType;
		}
		for (auto &i : gAttributes)
		{
			gAttributesMap[std::string(i.mName)] = i.mType;
		}
	}

	static ElementType getElementType(const char *str)
	{
		ElementType ret = ET_LAST;

		ElementTypeMap::iterator found = gElementsMap.find(std::string(str));
		if (found != gElementsMap.end())
		{
			ret = (*found).second;
		}
		return ret;
	}

	static AttributeType getAttributeType(const char *str)
	{
		AttributeType ret = AT_LAST;

		AttributeTypeMap::iterator found = gAttributesMap.find(std::string(str));
		if (found != gAttributesMap.end())
		{
			ret = (*found).second;
		}
		return ret;
	}
	static const char *getElementName(ElementType t)
	{
		const char *ret = "**UNKONWN-ELEMENT-TYPE**";
		if (t < ET_LAST)
		{
			ret = gElements[t].mName;
		}
		return ret;
	}

	static const char *getAttributeName(AttributeType t)
	{
		const char *ret = "**UNKONWN-ATTRIBUTE-TYPE**";
		if (t < AT_LAST)
		{
			ret = gAttributes[t].mName;
		}
		return ret;
	}


#define MAX_STACK 32

	class ImportPhysXDOMImpl : public ImportPhysXDOM, public FAST_XML::FastXml::Callback
	{
	public:
		ImportPhysXDOMImpl(void)
		{
			initMaps(); // initialize the look up tables

		}
		virtual ~ImportPhysXDOMImpl(void)
		{

		}

		void reportError(uint32_t lineno, const char *fmt, ...)
		{
			va_list         args;
			char            buffer[4096];
			va_start(args, fmt);
			STRING_HELPER::stringFormatV(buffer, sizeof(buffer), fmt, args);
			va_end(args);
			printf("[ImportPhysXDOM:ERROR:Lineno:%d]%s\n", lineno, buffer);
		}

		void nestingError(uint32_t lineno, ElementType type, ElementType expected, ElementType parent)
		{
			reportError(lineno, "Nesting error <%s> expected parent <%s> (or others) but found <%s>",
				getElementName(type),
				getElementName(expected),
				getElementName(parent));
		}


		// Imports an XML serialized asset and converts it into a standardized PhysicsDOM
		virtual bool importPhysXDOM(const char *xmlName,		// Name of the PhysX XML file
			PHYSICS_DOM::PhysicsDOM &dom) final	// The DOM to load it into
		{
			bool ret = false;

			PHYSICS_DOM::PhysicsDOM c;
			dom = c;	// empty it
			mImportDOM = &dom;

			FAST_XML::FastXml *f = FAST_XML::FastXml::create();
			f->processXml(xmlName, this);
			f->release();

			mImportDOM = nullptr;


			return ret;
		}


		// return true to continue processing the XML document, false to skip.
		virtual bool processElement(
			const char *elementName,   // name of the element
			uint32_t argc,         // number of attributes pairs
			const char **argv,         // list of attributes.
			const char  *elementData,  // element data, null if none
			uint32_t lineno) final  // line number in the source XML file
		{
			if (argc & 1) // if it's odd
			{
				reportError(lineno, "Attribute key/value pair mismatch");
				argc--;
			}
			uint32_t acount = argc / 2;
			mCurrentType = getElementType(elementName);
			mPreviousPreviousType = mStackLocation >= 2 ? mTypeStack[mStackLocation - 2] : ET_LAST;
			mPreviousType = mStackLocation ? mTypeStack[mStackLocation - 1] : ET_LAST;
			mTypeStack[mStackLocation] = mCurrentType;
			mStackLocation++;
			if (mStackLocation > MAX_STACK)
			{
				mStackLocation = MAX_STACK;
				reportError(lineno, "ElementTypes nested too deeply!");
			}
			for (uint32_t i = 0; i < acount; i++)
			{
				AttributeType atype = getAttributeType(argv[i * 2 + 0]);
				getAttributeName(atype);
			}

			switch (mCurrentType)
			{
			case ET_Rotation:
				break;
			default:
				reportError(lineno, "Unknown elementType(%s)", elementName);
				break;
			}
			return true;
		}

		virtual bool processComment(const char *comment)final  // encountered a comment in the XML
		{
			return true;
		}

		void reportCloseError(uint32_t lineno, ElementType type, ElementType expected, ElementType previousType)
		{
			reportError(lineno, "Got element-close <%s> without matching parent <%s> instead found <%s>",
				getElementName(type),
				getElementName(expected),
				getElementName(previousType));
		}

		// 'element' is the name of the element that is being closed.
		// depth is the recursion depth of this element.
		// Return true to continue processing the XML file.
		// Return false to stop processing the XML file; leaves the read pointer of the stream right after this close tag.
		// The bool 'isError' indicates whether processing was stopped due to an error, or intentionally canceled early.
		virtual bool processClose(const char *element, uint32_t depth, bool &isError, uint32_t lineno) final	  // process the 'close' indicator for a previously encountered element
		{
			// We pop the element type stack and revise the current and previous type variables
			if ((depth + 1) != mStackLocation)
			{
				reportError(lineno, "Element Stack is messed up.");
			}
			ElementType type = getElementType(element);
			if (mStackLocation)
			{
				mStackLocation--;
				if (mTypeStack[mStackLocation] != type)
				{
					reportError(lineno, "ElementClose did not match the previous element open! Invalid XML file.");
					mStackLocation++; // don't pop the stack, this was a mismatched close
					return true;
				}
				else
				{
					mPreviousPreviousType = ET_LAST;
					mCurrentType = mTypeStack[mStackLocation];
					if (mStackLocation)
					{
						mPreviousType = mTypeStack[mStackLocation - 1];
						if ((mStackLocation - 1))
						{
							mPreviousPreviousType = mTypeStack[mStackLocation - 2];
						}
					}
					else
					{
						mPreviousType = ET_LAST;
					}
				}
			}
			switch (type)
			{
			case ET_Rotation:
				break;
			}
			return true;
		}


	virtual void release(void) final
	{
		delete this;
	}

	// The DOM we are importing
	PHYSICS_DOM::PhysicsDOM	*mImportDOM{ nullptr };

	uint32_t			mStackLocation{ 0 };
	ElementType			mCurrentType{ ET_LAST };	// The current element type we are processing
	ElementType			mPreviousType{ ET_LAST };	// The previous element type (parent node type)
	ElementType			mPreviousPreviousType{ ET_LAST }; // two up the call stack
	ElementType			mTypeStack[MAX_STACK];
};

ImportPhysXDOM *ImportPhysXDOM::create(void)
{
	ImportPhysXDOMImpl *ret = new ImportPhysXDOMImpl;
	return static_cast<ImportPhysXDOM *>(ret);
}


} // End of IMPORT_PHYSX_DOM namespace
