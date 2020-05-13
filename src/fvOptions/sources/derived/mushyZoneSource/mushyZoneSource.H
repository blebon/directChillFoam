/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2014-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::fv::mushyZoneSource

Description
    This source is designed to model the effect of solidification and melting
    processes, e.g. windhield defrosting.  The phase change occurs between the
    liquidus and solidus temperatures, \c Tliquidus Tsolidus.

    The presence of the solid phase in the flow field is incorporated into the
    model as a momentum porosity contribution; the energy associated with the
    phase change is added as an enthalpy contribution.

    Based on the references:

      1. V.R. Voller and C. Prakash, A fixed grid numerical modelling
         methodology for convection-diffusion mushy phase-change problems,
         Int. J. Heat Mass Transfer 30(8):17091719, 1987.
      2. C.R. Swaminathan. and V.R. Voller, A general enthalpy model for
         modeling solidification processes, Metallurgical Transactions
         23B:651664, 1992.

    The model generates a field \c \<name\>_alpha1 which can be visualised to
    to show the melt distribution as a fraction [0-1]

Usage
    Example usage:
    \verbatim
    mushyZoneSource1
    {
        type            mushyZoneSource;
        active          yes;

        mushyZoneSourceCoeffs
        {
            selectionMode   cellZone;
            cellZone        iceZone;

            Tliquidus       273;
            Tsolidus        273;
            L               334000;
            thermoMode      thermo;
            beta            50e-6;
            rhoRef          800;
        }
    }
    \endverbatim

    Where:
    \table
        Property     | Description             | Required    | Default value
        Tliquidus    | Liquidus temperature [K] | yes         |
        Tsolidus     | Solidus temperature [K] | yes         |
        L            | Latent heat of fusion [J/kg] | yes    |
        relax        | Relaxation coeficient [0-1]  | no     | 0.1
        thermoMode   | Thermo mode [thermo|lookup] | yes     |
        rhoRef       | Reference (solid) density | yes       |
        T            | Name of temperature field | no        | T
        Cp           | Name of specific heat capacity field | no | Cp
        U            | Name of velocity field  | no          | U
        phi          | Name of flux field      | no          | phi
        Cu           | Model coefficient       | no          | 100000
        q            | Model coefficient       | no          | 0.001
        beta         | Thermal expansion coefficient [1/K] | yes |
        g            | Accelerartion due to gravity | no     |
    \endtable

SourceFiles
    mushyZoneSource.C
    mushyZoneSourceIO.C

\*---------------------------------------------------------------------------*/

#ifndef mushyZoneSource_H
#define mushyZoneSource_H

#include "fvMesh.H"
#include "volFields.H"
#include "cellSetOption.H"
#include "NamedEnum.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                 Class mushyZoneSource Declaration
\*---------------------------------------------------------------------------*/

class mushyZoneSource
:
    public cellSetOption
{
public:

    enum thermoMode
    {
        mdThermo,
        mdLookup
    };

    static const NamedEnum<thermoMode, 2> thermoModeTypeNames_;


private:

    // Private data

        //- Liquidus temperature [K]
        scalar Tliquidus_;

        //- Solidus temperature [K]
        scalar Tsolidus_;

        //- Latent heat of fusion [J/kg]
        scalar L_;

        //- Packing fraction
        scalar g_env_;

        //- Relaxation coefficient
        scalar relax_;

        //- Casting velocity [m/s]
        // vector castingVelocity_;
        volVectorField castingVelocity_;

        //- Thermodynamics mode
        thermoMode mode_;

        //- Reference density - typically the solid density
        scalar rhoRef_;

        //- Name of temperature field - default = "T" (optional)
        word TName_;

        //- Name of specific heat capacity field - default = "Cp" (optional)
        word CpName_;

        //- Name of velocity field - default = "U" (optional)
        word UName_;

        //- Name of flux field - default = "phi" (optional)
        word phiName_;

        //- Mushy region momentum sink coefficient [1/s]; default = 10^5
        scalar Cu_;

        //- Coefficient used in porosity calc - default = 0.001
        scalar q_;

        //- Thermal expansion coefficient [1/K]
        scalar beta_;

        //- Phase fraction indicator field
        volScalarField alpha1_;

        //- Current time index (used for updating)
        label curTimeIndex_;

        //- Temperature change cached for source calculation when alpha1 updated
        scalarField deltaT_;


    // Private Member Functions

        //- Return the specific heat capacity field
        tmp<volScalarField> Cp() const;

        //- Return the gravity vector
        vector g() const;

        //- Update the model
        void update();

        //- Helper function to apply to the energy equation
        template<class RhoFieldType>
        void apply(const RhoFieldType& rho, fvMatrix<scalar>& eqn);


public:

    //- Runtime type information
    TypeName("mushyZoneSource");


    // Constructors

        //- Construct from explicit source name and mesh
        mushyZoneSource
        (
            const word& sourceName,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );

        //- Disallow default bitwise copy construction
        mushyZoneSource
        (
            const mushyZoneSource&
        ) = delete;


    // Member Functions

        // Add explicit and implicit contributions

            //- Add explicit contribution to enthalpy equation
            virtual void addSup(fvMatrix<scalar>& eqn, const label fieldi);

            //- Add implicit contribution to momentum equation
            virtual void addSup(fvMatrix<vector>& eqn, const label fieldi);


        // Add explicit and implicit contributions to compressible equation

            //- Add explicit contribution to compressible enthalpy equation
            virtual void addSup
            (
                const volScalarField& rho,
                fvMatrix<scalar>& eqn,
                const label fieldi
            );

            //- Add implicit contribution to compressible momentum equation
            virtual void addSup
            (
                const volScalarField& rho,
                fvMatrix<vector>& eqn,
                const label fieldi
            );


        // IO

            //- Read source dictionary
            virtual bool read(const dictionary& dict);
        
        // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const mushyZoneSource&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "mushyZoneSourceTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
