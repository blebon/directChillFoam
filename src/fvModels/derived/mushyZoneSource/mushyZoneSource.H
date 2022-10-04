/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2014-2021 OpenFOAM Foundation
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
    processes, e.g. windshield defrosting.

    The isotherm phase change occurs at the melting temperature, \c Tsol (= \c
    Tliq). The not isotherm phase change occurs between solidus and liquidus
    temperature, \c Tsol < \c Tliq respectively.

    The presence of the solid phase in the flow field is incorporated into the
    model as a momentum porosity contribution; the energy associated with the
    phase change is added as an enthalpy contribution.

    The model generates the field \c \<name\>:alpha1 which can be visualised to
    to show the melt distribution as a fraction [0-1].

Usage
    Example usage:
    \verbatim
    mushyZoneSource1
    {
        type            mushyZoneSource;

        selectionMode   cellZone;
        cellZone        iceZone;

        Tsol            273;
        L               334000;
        thermoMode      thermo;
        beta            50e-6;
        rhoRef          800;
    }
    \endverbatim

    where:
    \table
        Property   | Description                     | Required | Default value
        Tsol       | Solidus temperature [K]             | yes      |
        Tliq       | Liquidus temperature [K]            | no       | Tsol
        L          | Latent heat of fusion [J/kg]        | yes      |
        g_env      | Packing fraction                    | no       | 0.7
        relax      | Relaxation coefficient [0-1]        | no       | 0.9
        castingVelocity | Casting velocity [m/s]         | yes      |
        tStar      | Reverse liquid fraction table       | yes      |
        thermoMode | Thermo mode [thermo|lookup]         | yes      |
        rhoRef     | Reference (solid) density [kg/m^3]  | yes      |
        rho        | Name of density field               | no       | rho
        T          | Name of temperature field           | no       | T
        Cp         | Name of specific heat field         | no       | Cp
        U          | Name of velocity field              | no       | U
        phi        | Name of flux field                  | no       | phi
        Cu         | Model coefficient [1/s]             | no       | 100000
        q          | Model coefficient                   | no       | 0.001
        beta       | Thermal expansion coefficient [1/K] | yes      |
        g          | Accelerartion due to gravity        | no       |
    \endtable

References
    - Voller, V. R., & Prakash, C. (1987). A fixed grid numerical modelling 
    methodology for convection-diffusion mushy region phase-change problems.
    _International Journal of Heat and Mass Transfer_, **30**(8), 1709-1719. [doi:10.1016/0017-9310(87)90317-6](https://doi.org/10.1016/0017-9310&#40;87&#41;90317-6)

    - Swaminathan, C. R., & Voller, V. R. (1992). A general enthalpy method 
    for modeling solidification processes. _Metallurgical transactions B_, **23**
    (5), 651-664. [doi:10.1007/BF02649725](https://doi.org/10.1007/BF02649725)

    - G.S. Bruno Lebon, Georges Salloum-Abou-Jaoude, Dmitry Eskin, Iakovos Tzanakis,
    Koulis Pericleous, Philippe Jarry (2019). Numerical modelling of acoustic 
    streaming during the ultrasonic melt treatment of direct-chill (DC) casting.
    _Ultrasonics Sonochemistry_, **54**, 171-182. [doi:10.1016/j.ultsonch.2019.02.002](https://doi.org/10.1016/j.ultsonch.2019.02.002)

SourceFiles
    mushyZoneSource.C

\*---------------------------------------------------------------------------*/

#ifndef mushyZoneSource_H
#define mushyZoneSource_H

#include "fvModel.H"
#include "fvCellSet.H"
#include "fvMesh.H"
#include "volFields.H"
#include "NamedEnum.H"
#include "Function1.H"

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
    public fvModel
{
public:

    enum class thermoMode
    {
        thermo,
        lookup
    };

    static const NamedEnum<thermoMode, 2> thermoModeTypeNames_;


private:

    // Private Data

        //- The set of cells the fvConstraint applies to
        fvCellSet set_;

        //- Temperature at which isotherm melting occurs
        //  and not isotherm melting starts e.g. eutectic [K]
        scalar Tsol_;

        //- Temperature at which not isotherm melting stops (Tliq > Tsol) [K]
        scalar Tliq_;

        //- Latent heat of fusion [J/kg]
        scalar L_;

        //- Packing fraction
        scalar g_env_;

        //- Phase fraction under-relaxation coefficient
        scalar relax_;

        //- Casting velocity [m/s]
        volVectorField castingVelocity_;

        //- Reverse liquid fraction table
        autoPtr<Function1<scalar>> tStar_;

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
        mutable volScalarField alpha1_;


    // Private Member Functions

        //- Non-virtual read
        void readCoeffs();

        //- Return the specific heat capacity field
        tmp<volScalarField> Cp() const;

        //- Return the gravity vector
        vector g() const;

        //- Update the model
        void update(const volScalarField& Cp) const;

        //- Helper function to apply to the energy equation
        template<class RhoFieldType>
        void apply(const RhoFieldType& rho, fvMatrix<scalar>& eqn) const;


public:

    //- Runtime type information
    TypeName("mushyZoneSource");


    // Constructors

        //- Construct from explicit source name and mesh
        mushyZoneSource
        (
            const word& name,
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

        // Checks

        //- Return the list of fields for which the fvModel adds source term to the transport equation.
        virtual wordList addSupFields() const;


        // Add explicit and implicit contributions

        //- Add explicit contribution to enthalpy equation
        virtual void addSup
        (
            fvMatrix<scalar>& eqn,
            const word& fieldName
        ) const;

        //- Add implicit contribution to momentum equation
        virtual void addSup
        (
            fvMatrix<vector>& eqn,
            const word& fieldName
        ) const;


        // Add explicit and implicit contributions to compressible equation

        //- Add explicit contribution to compressible enthalpy equation
        virtual void addSup
        (
            const volScalarField& rho,
            fvMatrix<scalar>& eqn,
            const word& fieldName
        ) const;

        //- Add implicit contribution to compressible momentum equation
        virtual void addSup
        (
            const volScalarField& rho,
            fvMatrix<vector>& eqn,
            const word& fieldName
        ) const;


        // Mesh motion

        //- Update for mesh changes
        virtual void updateMesh(const mapPolyMesh&);


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
