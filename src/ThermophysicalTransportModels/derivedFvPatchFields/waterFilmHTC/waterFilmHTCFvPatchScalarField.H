/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2020 OpenFOAM Foundation
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
    Foam::waterFilmHTCFvPatchScalarField

Description
This boundary condition applies a heat flux condition to temperature
on an external wall in one of three modes:    
    
    - fixed power: supply \c Q  
    - fixed heat flux: supply \c q  
    - fixed heat transfer coefficient: supply \c h and \c Ta

    .
where:
    \vartable
        Q  | Power [W]
        q  | Heat flux [W/m^2]
        h  | Heat transfer coefficient [W/m^2/K]
        Ta | Ambient temperature [K]
    \endvartable
    .
    For heat transfer coefficient mode, optional thin thermal layer resistances
    can be specified through thicknessLayers and kappaLayers entries.

    The thermal conductivity \c kappa can either be retrieved from various
    possible sources, as detailed in the class temperatureCoupledBase.

    The ambient temperature Ta is specified as a Foam::Function1 of time but
    uniform in space.

Usage
    \table
    Property     | Description                 | Required | Default value
    mode         | 'power', 'flux' or 'coefficient' | yes |
    Q            | Power [W]                   | for mode 'power'     |
    q            | Heat flux [W/m^2]           | for mode 'flux'     |
    htc          | Heat transfer coefficient [W/m^2/K] | for mode 'coefficent' |
    Ta           | Ambient temperature [K]     | for mode 'coefficient' |
    thicknessLayers | Layer thicknesses [m] | no |
    kappaLayers  | Layer thermal conductivities [W/m/K] | no |
    relaxation   | Relaxation for the wall temperature | no | 1
    emissivity   | Surface emissivity for radiative flux to ambient | no | 0
    qr           | Name of the radiative field | no | none
    qrRelaxation | Relaxation factor for radiative field | no | 1
    \endtable
    .
Example of the boundary condition specification:
    \verbatim
    <patchName>
    {
        type            waterFilmHTC;

        mode            coefficient;

        Ta              constant 300.0;
        htc             constant 10000.0;
        thicknessLayers (0.1 0.2 0.3 0.4);
        kappaLayers     (1 2 3 4);

        value           $internalField;
    }
    \endverbatim

See also
    Foam::temperatureCoupledBase
    Foam::mixedFvPatchScalarField
    Foam::mouldHTCFvPatchScalarField

SourceFiles
    waterFilmHTCFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#ifndef waterFilmHTCFvPatchScalarField_H
#define waterFilmHTCFvPatchScalarField_H

#include "mixedFvPatchFields.H"
#include "temperatureCoupledBase.H"
#include "Function1.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
      Class waterFilmHTCFvPatchScalarField Declaration
\*---------------------------------------------------------------------------*/

class waterFilmHTCFvPatchScalarField
:
    public mixedFvPatchScalarField,
    public temperatureCoupledBase
{
public:

    // Public data

        //- Operation mode enumeration
        enum operationMode
        {
            fixedPower,
            fixedHeatFlux,
            variableHeatTransferCoeff
        };

        static const NamedEnum<operationMode, 3> operationModeNames;


private:

    // Private Data

        //- Operation mode
        operationMode mode_;

        //- Heat power [W]
        scalar Q_;

        //- Heat flux [W/m^2]
        scalarField q_;

        //- Heat transfer coefficient [W/m^2K] interpolation table
        autoPtr<Function1<scalar>> htc_;

        //- Ambient temperature [K]
        autoPtr<Function1<scalar>> Ta_;

        //- Relaxation for the wall temperature (thermal inertia)
        scalar relaxation_;

        //- Optional surface emissivity for radiative transfer to ambient
        scalar emissivity_;

        //- Cache qr for relaxation
        scalarField qrPrevious_;

        //- Relaxation for qr
        scalar qrRelaxation_;

        //- Name of the radiative heat flux
        const word qrName_;

        //- Thickness of layers
        scalarList thicknessLayers_;

        //- Conductivity of layers
        scalarList kappaLayers_;


public:

    //- Runtime type information
    TypeName("waterFilmHTC");


    // Constructors

        //- Construct from patch and internal field
        waterFilmHTCFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        waterFilmHTCFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given
        //  waterFilmHTCFvPatchScalarField
        //  onto a new patch
        waterFilmHTCFvPatchScalarField
        (
            const waterFilmHTCFvPatchScalarField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Disallow copy without setting internal field reference
        waterFilmHTCFvPatchScalarField
        (
            const waterFilmHTCFvPatchScalarField&
        ) = delete;

        //- Copy constructor setting internal field reference
        waterFilmHTCFvPatchScalarField
        (
            const waterFilmHTCFvPatchScalarField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchScalarField> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchScalarField>
            (
                new waterFilmHTCFvPatchScalarField(*this, iF)
            );
        }


    // Member Functions

        // Access

            //- Allow manipulation of the boundary values
            virtual bool fixesValue() const
            {
                return false;
            }


        // Mapping functions

            //- Map (and resize as needed) from self given a mapping object
            //  Used to update fields following mesh topology change
            virtual void autoMap(const fvPatchFieldMapper&);

            //- Reverse map the given fvPatchField onto this fvPatchField
            //  Used to reconstruct fields
            virtual void rmap(const fvPatchScalarField&, const labelList&);


        // Evaluation functions

            //- Update the coefficients associated with the patch field
            virtual void updateCoeffs();


        // I-O

            //- Write
            void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
