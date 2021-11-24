all: central.cpp serverT.cpp serverS.cpp serverP.cpp clientA.cpp clientB.cpp
	g++ -o central central.cpp
	g++ -o serverT serverT.cpp
	g++ -o serverS serverS.cpp
	g++ -o serverP serverP.cpp
	g++ -o clientA clientA.cpp

.PHONY: central
central:
	./central

.PHONY: serverT
serverT:
	./serverT

.PHONY: serverS
serverS:
	./serverS

.PHONY: serverP
serverP:
	./serverP

.PHONY: clientA
clientA:
	./clientA

.PHONY: clientB
clientB:
	./clientB
