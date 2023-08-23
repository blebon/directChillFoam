default:
			cd applications/solvers/heatTransfer/directChillFoam/multicomponentAlloy && wclean && wmake libso
		  cd applications/solvers/heatTransfer/directChillFoam && wclean && wmake
			cd src/fvConstraints && wclean && wmake libso
			cd src/fvModels && wclean && wmake libso
			cd src/ThermophysicalTransportModels && wclean && wmake libso
			@echo "************ SOLVER COMPILED **********"
