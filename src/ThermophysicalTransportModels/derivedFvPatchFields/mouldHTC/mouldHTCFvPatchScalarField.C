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

\*---------------------------------------------------------------------------*/

#include "mouldHTCFvPatchScalarField.H"
#include "volFields.H"
#include "physicoChemicalConstants.H"
#include "addToRunTimeSelectionTable.H"

using Foam::constant::physicoChemical::sigma;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    template<>
    const char*
    NamedEnum
    <
        mouldHTCFvPatchScalarField::operationMode,
        3
    >::names[] =
    {
        "power",
        "flux",
        "coefficient"
    };
}

const Foam::NamedEnum
<
    Foam::mouldHTCFvPatchScalarField::operationMode,
    3
> Foam::mouldHTCFvPatchScalarField::operationModeNames;


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::mouldHTCFvPatchScalarField::
mouldHTCFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    mixedFvPatchScalarField(p, iF),
    temperatureCoupledBase(patch()),
    mode_(fixedHeatFlux),
    Q_(0),
    Ta_(),
    relaxation_(1),
    emissivity_(0),
    qrRelaxation_(1),
    qrName_("undefined-qr"),
    thicknessLayers_(),
    kappaLayers_()
{
    refValue() = 0;
    refGrad() = 0;
    valueFraction() = 1;
}


Foam::mouldHTCFvPatchScalarField::
mouldHTCFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    mixedFvPatchScalarField(p, iF),
    temperatureCoupledBase(patch(), dict),
    mode_(operationModeNames.read(dict.lookup("mode"))),
    Q_(0),
    Ta_(),
    relaxation_(dict.lookupOrDefault<scalar>("relaxation", 1)),
    emissivity_(dict.lookupOrDefault<scalar>("emissivity", 0)),
    qrRelaxation_(dict.lookupOrDefault<scalar>("qrRelaxation", 1)),
    qrName_(dict.lookupOrDefault<word>("qr", "none")),
    thicknessLayers_(),
    kappaLayers_()
{
    switch (mode_)
    {
        case fixedPower:
        {
            dict.lookup("Q") >> Q_;

            break;
        }
        case fixedHeatFlux:
        {
            q_ = scalarField("q", dict, p.size());

            break;
        }
        case variableHeatTransferCoeff:
        {
            h_ = scalarField("h1", dict, p.size());
            h1_ = scalarField("h1", dict, p.size());
            h2_ = scalarField("h2", dict, p.size());
            Ta_ = Function1<scalar>::New("Ta", dict);

            if (dict.found("thicknessLayers"))
            {
                dict.lookup("thicknessLayers") >> thicknessLayers_;
                dict.lookup("kappaLayers") >> kappaLayers_;
            }

            break;
        }
    }

    fvPatchScalarField::operator=(scalarField("value", dict, p.size()));

    if (qrName_ != "none")
    {
        if (dict.found("qrPrevious"))
        {
            qrPrevious_ = scalarField("qrPrevious", dict, p.size());
        }
        else
        {
            qrPrevious_.setSize(p.size(), 0);
        }
    }

    if (dict.found("refValue"))
    {
        // Full restart
        refValue() = scalarField("refValue", dict, p.size());
        refGrad() = scalarField("refGradient", dict, p.size());
        valueFraction() = scalarField("valueFraction", dict, p.size());
    }
    else
    {
        // Start from user entered data. Assume fixedValue.
        refValue() = *this;
        refGrad() = 0;
        valueFraction() = 1;
    }
}


Foam::mouldHTCFvPatchScalarField::
mouldHTCFvPatchScalarField
(
    const mouldHTCFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    mixedFvPatchScalarField(ptf, p, iF, mapper),
    temperatureCoupledBase(patch(), ptf),
    mode_(ptf.mode_),
    Q_(ptf.Q_),
    Ta_(ptf.Ta_, false),
    relaxation_(ptf.relaxation_),
    emissivity_(ptf.emissivity_),
    qrRelaxation_(ptf.qrRelaxation_),
    qrName_(ptf.qrName_),
    thicknessLayers_(ptf.thicknessLayers_),
    kappaLayers_(ptf.kappaLayers_)
{
    switch (mode_)
    {
        case fixedPower:
        {
            break;
        }
        case fixedHeatFlux:
        {
            mapper(q_, ptf.q_);
            break;
        }
        case variableHeatTransferCoeff:
        {
            mapper(h_, ptf.h_);
            mapper(h1_, ptf.h1_);
            mapper(h2_, ptf.h2_);
            break;
        }
    }

    if (qrName_ != "none")
    {
        mapper(qrPrevious_, ptf.qrPrevious_);
    }
}


Foam::mouldHTCFvPatchScalarField::
mouldHTCFvPatchScalarField
(
    const mouldHTCFvPatchScalarField& tppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    mixedFvPatchScalarField(tppsf, iF),
    temperatureCoupledBase(patch(), tppsf),
    mode_(tppsf.mode_),
    Q_(tppsf.Q_),
    q_(tppsf.q_),
    h_(tppsf.h_),
    h1_(tppsf.h1_),
    h2_(tppsf.h2_),
    Ta_(tppsf.Ta_, false),
    relaxation_(tppsf.relaxation_),
    emissivity_(tppsf.emissivity_),
    qrPrevious_(tppsf.qrPrevious_),
    qrRelaxation_(tppsf.qrRelaxation_),
    qrName_(tppsf.qrName_),
    thicknessLayers_(tppsf.thicknessLayers_),
    kappaLayers_(tppsf.kappaLayers_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::mouldHTCFvPatchScalarField::autoMap
(
    const fvPatchFieldMapper& m
)
{
    mixedFvPatchScalarField::autoMap(m);

    switch (mode_)
    {
        case fixedPower:
        {
            break;
        }
        case fixedHeatFlux:
        {
            m(q_, q_);

            break;
        }
        case variableHeatTransferCoeff:
        {
            // m(h_, h_);
            m(h1_, h1_);
            m(h2_, h2_);

            break;
        }
    }

    if (qrName_ != "none")
    {
        m(qrPrevious_, qrPrevious_);
    }
}


void Foam::mouldHTCFvPatchScalarField::rmap
(
    const fvPatchScalarField& ptf,
    const labelList& addr
)
{
    mixedFvPatchScalarField::rmap(ptf, addr);

    const mouldHTCFvPatchScalarField& tiptf =
        refCast<const mouldHTCFvPatchScalarField>(ptf);

    switch (mode_)
    {
        case fixedPower:
        {
            break;
        }
        case fixedHeatFlux:
        {
            q_.rmap(tiptf.q_, addr);

            break;
        }
        case variableHeatTransferCoeff:
        {
            // h_.rmap(tiptf.h_, addr);
            h1_.rmap(tiptf.h1_, addr);
            h2_.rmap(tiptf.h2_, addr);

            break;
        }
    }

    if (qrName_ != "none")
    {
        qrPrevious_.rmap(tiptf.qrPrevious_, addr);
    }
}


void Foam::mouldHTCFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const scalarField& Tp(*this);
    scalarField fL_(Tp.size(), 0.0);

    // Store current valueFraction and refValue for relaxation
    const scalarField valueFraction0(valueFraction());
    const scalarField refValue0(refValue());

    scalarField qr(Tp.size(), 0);
    if (qrName_ != "none")
    {
        qr =
            qrRelaxation_
           *patch().lookupPatchField<volScalarField, scalar>(qrName_)
          + (1 - qrRelaxation_)*qrPrevious_;

        qrPrevious_ = qr;
    }

    switch (mode_)
    {
        case fixedPower:
        {
            refGrad() = (Q_/gSum(patch().magSf()) + qr)/kappa(*this);
            refValue() = Tp;
            valueFraction() = 0;

            break;
        }
        case fixedHeatFlux:
        {
            refGrad() = (q_ + qr)/kappa(*this);
            refValue() = Tp;
            valueFraction() = 0;

            break;
        }
        case variableHeatTransferCoeff:
        {
            scalar totalSolidRes = 0;
            if (thicknessLayers_.size())
            {
                forAll(thicknessLayers_, iLayer)
                {
                    const scalar l = thicknessLayers_[iLayer];
                    if (kappaLayers_[iLayer] > 0)
                    {
                        totalSolidRes += l/kappaLayers_[iLayer];
                    }
                }
            }

            fL_ = patch().lookupPatchField<volScalarField, scalar>("melt1_alpha1");

            forAll (h_, i)
            {
                h_[i] = h1_[i]*fL_[i] + h2_[i]*(1.0 - fL_[i]);
            }

            const scalar Ta = Ta_->value(this->db().time().timeOutputValue());
 
            const scalarField hp
            (
                1
               /(
                    1
                   /(
                        (emissivity_ > 0)
                      ? (
                            h_
                          + emissivity_*sigma.value()
                           *((pow3(Ta) + pow3(Tp)) + Ta*Tp*(Ta + Tp))
                        )()
                      : h_
                    ) + totalSolidRes
                )
            );

            const scalarField hpTa(hp*Ta);

            const scalarField kappaDeltaCoeffs
            (
                this->kappa(*this)*patch().deltaCoeffs()
            );

            refGrad() = 0;
 
            forAll(Tp, i)
            {
                if (qr[i] < 0)
                {
                    const scalar hpmqr = hp[i] - qr[i]/Tp[i];

                    refValue()[i] = hpTa[i]/hpmqr;
                    valueFraction()[i] = hpmqr/(hpmqr + kappaDeltaCoeffs[i]);
                }
                else
                {
		    refValue()[i] = (hpTa[i] + qr[i])/hp[i];
                    valueFraction()[i] = hp[i]/(hp[i] + kappaDeltaCoeffs[i]);
                }
            }

            break;
        }
    }

    valueFraction() =
        relaxation_*valueFraction()
      + (1 - relaxation_)*valueFraction0;

    refValue() = relaxation_*refValue() + (1 - relaxation_)*refValue0;

    mixedFvPatchScalarField::updateCoeffs();

    if (debug)
    {
        const scalar Q = gSum(kappa(*this)*patch().magSf()*snGrad());

        Info<< patch().boundaryMesh().mesh().name() << ':'
            << patch().name() << ':'
            << this->internalField().name() << " :"
            << " heat transfer rate:" << Q
            << " walltemperature "
            << " min:" << gMin(*this)
            << " max:" << gMax(*this)
            << " avg:" << gAverage(*this)
            << endl;
    }
}


void Foam::mouldHTCFvPatchScalarField::write
(
    Ostream& os
) const
{
    fvPatchScalarField::write(os);

    writeEntry(os, "mode", operationModeNames[mode_]);
    temperatureCoupledBase::write(os);

    switch (mode_)
    {
        case fixedPower:
        {
            writeEntry(os, "Q", Q_);

            break;
        }
        case fixedHeatFlux:
        {
            writeEntry(os, "q", q_);

            break;
        }
        case variableHeatTransferCoeff:
        {
            // writeEntry(os, "h", h_);
            writeEntry(os, "h1", h1_);
            writeEntry(os, "h2", h2_);
            writeEntry(os, Ta_());

            if (relaxation_ < 1)
            {
                writeEntry(os, "relaxation", relaxation_);
            }

            if (emissivity_ > 0)
            {
                writeEntry(os, "emissivity", emissivity_);
            }

            if (thicknessLayers_.size())
            {
                writeEntry(os, "thicknessLayers", thicknessLayers_);
                writeEntry(os, "kappaLayers", kappaLayers_);
            }

            break;
        }
    }

    writeEntry(os, "qr", qrName_);

    if (qrName_ != "none")
    {
        writeEntry(os, "qrRelaxation", qrRelaxation_);

        writeEntry(os, "qrPrevious", qrPrevious_);
    }

    writeEntry(os, "refValue", refValue());
    writeEntry(os, "refGradient", refGrad());
    writeEntry(os, "valueFraction", valueFraction());
    writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        mouldHTCFvPatchScalarField
    );
}

// ************************************************************************* //
