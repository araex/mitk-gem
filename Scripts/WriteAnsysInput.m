%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% WriteAnsysInput.m, vers. 1.1, 17.08.2017
% Copyright, Benedikt Helgason and ETH-Zurich, Switzerland.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%This function writes Ansys Mechanical input files in APDL language.
%
%The function creates several files.
%RunAnsysModelX.inp (X = A, B, C, D or E). This is the Ansys master file.
%
%AnsysMeshX.cdb (X = A, B, C, D or E). This is the Ansys mesh file.
%
%MaterialPropsX.inp (X = A, B, C, D or E). This is the Ansys material
%property file.
%
%PlotModulusX.inp (X = A, B, C, D or E). This is a file for plotting the
%material property distribution in Ansys.
%
%SimpleBCs.inp. This Ansys input file allows the Ansys master file to run
%a simple load case so that material property distribution can be plotted.
%This is commented out in this MATLAB script but you can switch it on if
%you are analyzing a different dataset from that in the MITK-GEM tutorial.
%
%SimpleSWFBCs.inp. This Ansys input file allows the Ansys master file to run
%a simple sideways fall load case to estimate whole bone femoral strength.
%You should switch it off if you are not analyzing the dataset used in the
%MITK-GEM tutorial.
%
%PosProcess.inp. This Ansys input file asseses whole bone femoral strength
%for the sample model generated in the MITK-GEM tutorial. BE AWARE THAT
%THIS APPROACH FOR ASSESSING WHOLE BONE STRENGTH HAS NOT VALIDATED ANS IS
%ONLY INCLUDED HERE FOR DEMONSTRATION PURPOSES.
%
%
%You can run the Ansys master script in Ansys Classic Mechanical with the
%command *USE,RunAnsysModelX.inp where X = A, B, C, D or E
%
%In Ansys the FE mesh will be loaded and materials mapped to elements
%or nodes depending on the material mapping method the user has selected
%in the MITK-GEM.  The function has been tested on human femur FE models
%based on segmenting clinical CT data. The function may not be robust for
%other organs or datasets.
%
%
%INPUT
%GEMfilename: the complete path and filename for the text file with the
%model data that MITK-GEM outputs.
%
%Method: input 'A', 'B', 'C', 'D' and 'E' where A-E refers to the material
%mapping method in Helgason et al. 2016, Med Eng and Phys 38:679-689
%
%NumOfMats: Refers to the total number of material cards used for the FEM.
%A typical number would be e.g. 500 for a human proximal femur FEM.
%
%Ecort: Refers to the cortical bone E-modulus used in method D. In the MEP
%paper this is defined as the E-modulus of your modulus-density relation-
%ship at rho_app = 1.8 g/cm3.
%
%EminShell: Refers to the minimum E-modulus used for shell elements when
%method E is used. In Bessho et al. 2007, JBM 40:1745-1753 this value
%was set to 10 GPa.
%
%ShellTh: The shell thickness used for the shell elements when method E is
%used. In Bessho et al. 2007, JBM 40:1745-1753 this value was set to
%0.4 mm which was equal to the pixel width of their CT data (0.398 mm).
%
%GENERAL COMMENTS
%The plotting script PlotModulusX.inp is not necessary for loading the
%models into Ansys. This is just provided by the WriteAnsysInput.m function
%to allow for easy plotting of the material property distribution after
%the model has been solved. You do that in Ansys classic mechanical by
%going into /POST1 and plot Body Temperatures which are equal to the
%E-modulus for all the material mapping methods.
%
%There is also a simple boundary condition script (SimpleBCS.inp)
%in the RunAnsysModelX.inp.  This is to allow the script to run a fake
%Ansys simulation step that subsequntly allows for plotting of the material
%property distribution.  Replace this BC script with BCs that apply for
%your model.
%
%For material mapping methods C and D, a initial simulation step is
%included in the RunAnsysModelC.inp and RunAnsysModelD.inp where
%temperature (equal to Young's modulus) is assigned to all the nodes. This
%initial step, with no mechanical load, is used to make sure that ANSYS
%does not ramp the temperature load with the ANSYS timesteps. This ensures
%that the full temperature is applied to the nodes (and thus the Young's
%modulus) when the mechanical load is applied to the model in the second
%loading step.
%
%Below is one example for input variables for WriteAnsysInput for the case
%that your MITK-GEM text file (in this case: material mapped mesh.txt) is
%in same directory as the WriteAnsysInput.m script.
%WriteAnsysInput(['material mapped mesh.txt'],'A',500,[],[],[])
%WriteAnsysInput(['material mapped mesh.txt'],'B',500,[],[],[])
%WriteAnsysInput(['material mapped mesh.txt'],'C',[],16446,[],[])
%WriteAnsysInput(['material mapped mesh.txt'],'D',[],16446,[],[])
%WriteAnsysInput(['material mapped mesh.txt'],'E',500,16446,10000,0.4)
%
%
function WriteAnsysInput(GEMfilename,Method,NumOfMats,Ecort,EminShell,ShellTh)

if exist(GEMfilename,'file')==0
    error('*****Can not find input file %s*****',GEMfilename)
end


%----------- BEGIN READING INPUT FILE FROM MITK-GEM ----------
%----------- BEGIN READING INPUT FILE FROM MITK-GEM ----------
fprintf('*\n')
fprintf('*****READING ANSYS INPUT FILE*****\n')
fprintf('*\n')
[PathName,FileName,ext]=fileparts(GEMfilename);
FileName = [FileName,ext];

if isnumeric(FileName)
    error('*****Operation Cancelled*****\n\n')
end

% ******************OPEN FILE*****************
fprintf('*****INPUT FILE: %s*****\n',FileName)
fprintf('*\n')
fid0=fopen(FileName);


% ******************READ NODES*****************
tline='!!!';
NODES = [];
fprintf('*****READING NODE DATA*****\n')
fprintf('*\n')
while length(tline)<6 || ~strcmpi(tline(1:12),'#BEGIN NODES')
    tline=fgetl(fid0);
end
tline=fgetl(fid0); % Reading 1 more header line
tline=fgetl(fid0); % Reading 1 more header line
NODES = cell2mat(textscan(fid0, '%f,%f,%f,%f,%f', 'EndOfLine','\r\n'));
fgetl(fid0); % Reading '#END NODES'


% ******************READ ELEMENTS*****************
tline='!!!';
ELEMENTS = [];
fprintf('*****READING ELEMENT DATA*****\n')
fprintf('*\n')
while length(tline)<6 || ~strcmpi(tline(1:15),'#BEGIN ELEMENTS')
    tline=fgetl(fid0);
end
tline=fgetl(fid0); % Reading 1 more header line
tline=fgetl(fid0); % Reading 1 more header line
ELEMENTS = cell2mat(textscan(fid0, '%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f', 'EndOfLine','\r\n'));
fgetl(fid0); % Reading '#END ELEMENTS'

% ******************READ SURFACES*****************
tline='!!!';
SURFACES = [];
fprintf('*****READING EXTERNAL SURFACE DATA*****\n')
fprintf('*\n')
while length(tline)<6 || ~strcmpi(tline(1:14),'#BEGIN SURFACE')
    tline=fgetl(fid0);
end
tline=fgetl(fid0); % Reading 1 more header line
SURFACES = cell2mat(textscan(fid0, '%f,%f,%f,%f,%f,%f,%f', 'EndOfLine','\r\n'));
fgetl(fid0); % Reading '#END SURFACE'
fclose(fid0);


% ******************PLACING MATARIALS INTO NumOfMats BINS*****************
switch Method
    case 'A'
        %Write the Method A material file
        El_E = ELEMENTS(:,12);
        El_Mat = WriteMatFile(El_E,NumOfMats,'A',EminShell,Ecort,ShellTh,SURFACES);
    case 'B'
        %Write the Method B material file
        El_E = ELEMENTS(:,13);
        El_Mat = WriteMatFile(El_E,NumOfMats,'B',EminShell,Ecort,ShellTh,SURFACES);
    case 'C'
        %Write the Method C material file
        WriteMatFile([],NumOfMats,'C',EminShell,Ecort,ShellTh,SURFACES,NODES);
        El_Mat = ones(length(ELEMENTS(:,1)),1);
    case 'D'
        %Write the Method D material file
        WriteMatFile([],NumOfMats,'D',EminShell,Ecort,ShellTh,SURFACES,NODES);
        El_Mat = ones(length(ELEMENTS(:,1)),1);
    case 'E'
        %Write the Method E material file
        El_E = ELEMENTS(:,12);

        if (isempty(EminShell))
            error('*****Min Cortical Young's modulus needs to greater than 0*****')
        end
        if (EminShell <= 0)
            error('*****Min Cortical Young's modulus needs to greater than 0*****')
        end
        if isempty(ShellTh)
            error('*****Shell thickness needs to be larger than 0 for method E*****')
        end
        if ShellTh <= 0
            error('*****Shell thickness needs to be larger than 0 for method E*****')
        end
        [El_Mat,E_Mat] = WriteMatFile(El_E,NumOfMats,'E',EminShell,Ecort,ShellTh,SURFACES,NODES);
        WriteShellFile(EminShell,ShellTh,NumOfMats,El_Mat,E_Mat,SURFACES,NODES)
end


%----------- WRITE AnsysMeshX.CDB ----------
%----------- WRITE AnsysMeshX.CDB ----------
% X refers to A, B, C, D or E
switch Method
    case 'A'
        MeshFile='AnsysMeshA.cdb';
    case 'B'
        MeshFile='AnsysMeshB.cdb';
    case 'C'
        MeshFile='AnsysMeshC.cdb';
    case 'D'
        MeshFile='AnsysMeshD.cdb';
    case 'E'
        MeshFile='AnsysMeshE.cdb';
    end
fid0=fopen(MeshFile,'w');

fprintf('*****WRITING FE MESH To %s*****\n',MeshFile)
fprintf('*\n')

fprintf(fid0,'!! ANSYS input generated in MATLAB by %s\r\n','Ansys_mesh.cdb');
fprintf(fid0,'!! This file was created on %s\r\n\r\n',datestr(clock));


% ******************WRITE NODES*****************
fprintf(fid0,'\r\n/COM,*********** NODE DEFINITIONS ***********\r\n');
fprintf(fid0,'/PREP7\n');
fprintf(fid0,'NBLOCK,3\r\n(1i8,3e20.9)\r\n');
fprintf(fid0,'%8u%20.9f%20.9f%20.9f\r\n',NODES(:,1:4)');
fprintf(fid0,'N,R5.3,LOC,     -1,\r\n');


% ******************WRITE ELEMENTS*****************
fprintf(fid0,'\r\n/COM,*********** ELEMENT DEFINITIONS ***********\r\n');
fprintf(fid0,'ET,1,%u\r\n',187);
fprintf(fid0,'EBLOCK,%u\r\n',10);
fprintf(fid0,'(15i9)\n');
NE = length(ELEMENTS(:,1));

%Field 1 - The element number.
%Field 2 - The type of section ID.
%Field 3 - The real constant number.
%Field 4 - The material number.
%Field 5 - The element coordinate system number.
%Fields 6-15 - The node numbers. The next line will have the additional node numbers if there are more than ten.
el_input = [ELEMENTS(:,1), ones(NE,1), zeros(NE,1), El_Mat, zeros(NE,1), ELEMENTS(:,2:11)];

for i = 1:NE
    fprintf(fid0,'%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i\r\n',el_input(i,:));
end
fprintf(fid0,'%8d\r\n',-1);
fprintf(fid0,'ALLSEL,ALL\r\n');
fprintf(fid0,'CM,GemEls,ELEM\r\n');
fclose(fid0);


%----------- WRITE RunAnsysMethodX.inp ----------
%----------- WRITE RunAnsysMethodX.inp ----------
% X refers to A, B, C, D or E
WriteMasterScript(Method);


%----------- WRITE PlotModulusX.inp ----------
%----------- WRITE PlotModulusX.inp ----------
% X refers to A, B, C, D or E
WritePlotScript(Method);


%----------- WRITE SimpleBCs.inp ----------
%----------- WRITE SimpleBCs.inp ----------
% X refers to A, B, C, D or E
WriteSimpleBCs;



%----------- WRITE SimpleSWFBCs.inp ----------
%----------- WRITE SimpleSWFBCs.inp ----------
% X refers to A, B, C, D or E
WriteSimpleSWFBCs;



%----------- WRITE PostProcess.inp ----------
%----------- WRITE PostProcess.inp ----------
WritePostProcess;


%----------- Functions used by WriteAnsysInput.m ----------
%----------- Functions used by WriteAnsysInput.m ----------
%----------- Functions used by WriteAnsysInput.m ----------
%----------- Functions used by WriteAnsysInput.m ----------

%Function that writes the material property files for Anays
function [El_Mat,E_Mat] = WriteMatFile(El_E,NumOfMats,Method,EminShell,Ecort,ShellTh,SURFACES,NODES);
    switch Method
        case 'A'
            MatFile='MaterialPropsA.inp';
        case 'B'
            MatFile='MaterialPropsB.inp';
        case 'C'
            MatFile='MaterialPropsC.inp';
        case 'D'
            MatFile='MaterialPropsD.inp';
        case 'E'
            MatFile='MaterialPropsE.inp';
    end
    fprintf(['*****GENERATING MaterialProps',Method,'.inp*****\n'])
    fprintf('*\n')
    El_Mat = 0;
    E_Mat = 0;
    if strcmp(Method,'A') | strcmp(Method,'B') | strcmp(Method,'E')
    %Assign a material card to each element
    %MatEls is the number of elements in each Material Card
    %El_Mat is the Material Card Number for each element
    %[N,BIN] = HISTC(X,EDGES,...) where BIN is an index matrix
    %E_Mat, which has the length=NumOfMats, includes the E-mod for each
    %material card
    %Length of MatEls is NumOfMats+1
    E_Mat=linspace(min(El_E),max(El_E),NumOfMats)';
    [MatEls,El_Mat] = histc(El_E,[-inf,(E_Mat(1:end-1)'+E_Mat(2:end)')/2,inf]);
    UsedMat=MatEls(1:end-1)>0;   %Material indices used by at least 1 element

    %UsedMat contains the E-moduli for only the materials that are used
    %The length is equal to NumOfMats minus the number of material cards
    %not used.
    %Switch this on if you want to exclude material cards in your model
    %not used by any elements. If you don't switch this on the number of
    %material cards in your model will be equal to the NumOfMats.
    %E_Mat(~UsedMat)=[];

    %UsedMat herefter contains the reduced material card list, length is 500 minus
    %number of material cards not used
    UsedMat=find(UsedMat);

    %MInd has length 500 with zeros added for material cards not used
    MInd(UsedMat)=1:length(UsedMat);

    %Length of this one is = number of elements
    %El_Mat contains the material number for each element
    El_Mat=MInd(El_Mat)';

    %fprintf('Writing material properties on %s\n',MatFile)
    fid0 = fopen(MatFile,'w');

    fprintf(fid0,'/PREP7\r\n');
    fprintf(fid0,'ALLSEL,ALL\r\n');
    fprintf(fid0,'*MSG,INFO\r\n');
    fprintf(fid0,'ASSIGNING MATERIAL PROPERTIES. PLEASE WAIT...\r\n');
    fprintf(fid0,'/NOPR         !Disable output\r\n');
    fprintf(fid0,'\r\n!These material properties defined in Matlab!\r\n');

    for j = 1:length(E_Mat)
        fprintf(fid0,['\r\n!*---------------MATERIAL %u '...
            '(Used by %u elements)------------------\r\n'],j,MatEls(j));
        fprintf(fid0,'MP,EX,%u,%.15G\r\n',j,E_Mat(j));
        fprintf(fid0,'MP,NUXY,%u,0.30\r\n',j);
    end
    fclose(fid0);
    end

    if strcmp(Method,'C') | strcmp(Method,'D')
        nodeTEMPS = NODES(:,5);
        fid0 = fopen(MatFile,'w');
        fprintf(fid0,'!DEFINING MATERIAL PROPERTIES\r\n');
        fprintf(fid0,'max_NodeE = \r\n');
        fprintf(fid0,'max_NodeE = %.15G\r\n',max(nodeTEMPS));
        fprintf(fid0,'NNN = \r\n');
        fprintf(fid0,'NNN = 10\r\n');
        fprintf(fid0,'tem = \r\n');
        fprintf(fid0,'*DIM,factor,array,NNN\r\n');
        %We define a 10 MPa min modules, otherwise Ansys will complain
        %about negative or zero Young's modulus being used.
        fprintf(fid0,'*SET,tem,-50000,10,2392,4735,7077,8000,8500,9000,max_NodeE,50000\r\n');
        fprintf(fid0,'factor(1) = 1.0*10/max_NodeE\r\n');
        fprintf(fid0,'factor(10) = 1.0*max_NodeE/max_NodeE\r\n');
        fprintf(fid0,'*DO,i,2,NNN-1\r\n');
            fprintf(fid0,'factor(i) = 1.0*tem(i)/max_NodeE\r\n');
        fprintf(fid0,'*ENDDO\r\n');
        fprintf(fid0,'!DEFINING MATERIAL PROPERTIES\r\n');
        fprintf(fid0,'!DEFINING DATA TABLES\r\n');
        fprintf(fid0,'*DO,i,1,NNN\r\n');
            fprintf(fid0,'MPTEMP,i,tem(i)\r\n');
            fprintf(fid0,'E1 = factor(i)*max_NodeE\r\n');
            fprintf(fid0,'MPDATA,EX,1,,E1\r\n');
            fprintf(fid0,'MPDATA,PRXY,1,,0.3\r\n');
        fprintf(fid0,'*ENDDO\r\n');
        fprintf(fid0,'!*---------------------------------------------\r\n');

        fprintf(fid0,'ALLSEL,ALL\r\n');
        fprintf(fid0,'/GOPR     !Enable output\r\n');

        %Writing the Nodal temperature file
        if strcmp(Method,'C')
            TempFile = 'NodalTempsC.inp';
        end
        if strcmp(Method,'D')
            surfaceNODES = unique(SURFACES(:,2:7));
            nodeTEMPS(surfaceNODES) = Ecort;
            TempFile = 'NodalTempsD.inp';
        end

        disp(['*****GENERATING ',TempFile,'*****'])
        disp('*')
        fid0 = fopen(TempFile,'w');
        fprintf(fid0,'/PREP7\r\n');
        fprintf(fid0,'ALLSEL,ALL\r\n');
        fprintf(fid0,'*MSG,INFO\r\n');
        fprintf(fid0,'ASSIGNING MATERIAL PROPERTIES. PLEASE WAIT...\r\n');
        fprintf(fid0,'/NOPR         !Disable output\r\n');
        fprintf(fid0,'\r\n!These material properties defined in Matlab!\r\n');

        fprintf(fid0,'E_cort = \r\n');
        fprintf(fid0,'E_cort = %.15G\r\n',Ecort);
        fprintf(fid0,'bfdele,all,all\r\n');

        for i = 1:length(NODES(:,1))
            fprintf(fid0,'bf,%u,temp,%.15G\r\n',NODES(i,1),nodeTEMPS(i));
        end
    end



%******************MODELLING THE CORTICAL SHELL for METHOD E************
function WriteShellFile(EminShell,ShellTh,NumOfMats,El_Mat,E_Mat,SURFACES,NODES);
    %Writing the cortical shell materials to file
    NE = length(El_Mat);
    disp(['*****GENERATING ShellMaterialsE.inp','*****'])
    disp('*')
    fid0 = fopen('ShellMaterialsE.inp','w');
    fprintf(fid0,['\r\n!*--------------- Min E for shells ----------------\r\n']);
    fprintf(fid0,['\r\n!*---------------MATERIAL %u ------------------\r\n'],NumOfMats+1);
    fprintf(fid0,'MP,EX,%u,%.15G\r\n',NumOfMats+1,EminShell);
    fprintf(fid0,'MP,NUXY,%u,0.30\r\n',NumOfMats+1);
    fclose(fid0);

    %Writing the shell element mesh to file
    disp(['*****GENERATING ShellMeshE.cdb*****'])
    disp('*')
    fid0 = fopen('ShellMeshE.cdb','w');
    fprintf(fid0,'\r\n/COM,*********** SURFACE ELEMENT DEFINITIONS ***********\r\n');
    ShellType = 2;
    RealNr = 2;
    SecNr = 2;
    fprintf(fid0,'REAL,%u\r\n',RealNr);
    fprintf(fid0,'SECTYPE,%u,SHELL,,CortSec\r\n',SecNr);
    fprintf(fid0,'SECNUM,%u\r\n',SecNr);
    fprintf(fid0,'SECDATA,%.15G,%u\r\n',ShellTh,NumOfMats+1);
    fprintf(fid0,'ET,%u,%u\r\n',ShellType,281);
    fprintf(fid0,'EBLOCK,%u\r\n',8);
    fprintf(fid0,'(13i9)\n');
    NES = length(SURFACES(:,1));

    %Length of El_Mat is = number of elements
    %El_Mat contains the material number for each element
    surf_El_Mat = (NumOfMats+1)*ones(NES,1);
    for i = 1:NES
        Esolid = E_Mat(El_Mat(SURFACES(i,1)));
        %Esolid = El_Mat(SURFACES(i,1));
        if Esolid > EminShell
            surf_El_Mat(i) = El_Mat(SURFACES(i,1));
        end
    end
    %Field 1 - The element number.
    %Field 2 - The type of section ID.
    %Field 3 - The real constant number.
    %Field 4 - The material number.
    %Field 5 - The element coordinate system number.
    %Fields 6-15 - The node numbers. The next line will have the additional node numbers if there are more than ten.

    %The order of the surface nodes in the TXT file is not the same as
    %the order that ANSYS needs. This is fixed with the ind variable.
    ind = [1 3 2 6 5 4];
    reSURFACES = SURFACES(:,2:7);
    reSURFACES = reSURFACES(:,ind);
    reSURFACES = [reSURFACES(:,1:2) reSURFACES(:,3) reSURFACES(:,3) reSURFACES(:,4:5) reSURFACES(:,3) reSURFACES(:,6)];
    surf_el_input = [[NE+1:NE+NES]', SecNr*ones(NES,1), RealNr*ones(NES,1), surf_El_Mat, ...
        zeros(NES,1),  reSURFACES];

    for i = 1:NES
        fprintf(fid0,'%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i%9i\r\n',surf_el_input(i,:));
    end
    fprintf(fid0,'%8d\r\n',-1);
    fprintf(fid0,'ALLSEL,ALL\r\n');

    fprintf(fid0,'ESEL,S,TYPE,,2\r\n');

    fprintf(fid0,'CM,ShellElements,ELEM\r\n');
    fprintf(fid0,'ALLSEL,ALL\r\n');
    fclose(fid0);


 %******************Writing RunAnsysModelX.inp************
 % X refers to A, B, C, D or E
 function WriteMasterScript(Method);
    switch Method
        case 'A'
            disp(['*****GENERATING RunAnsysModelA.inp*****'])
            disp('*')
            fid0 = fopen('RunAnsysModelA.inp','w');
            fprintf(fid0,'finish\r\n');
            fprintf(fid0,'/CLEAR,ALL\r\n');
            fprintf(fid0,'/PREP7\r\n');
            fprintf(fid0,'*USE,AnsysMeshA.cdb\r\n');
            fprintf(fid0,'*USE,MaterialPropsA.inp\r\n');
            fprintf(fid0,'*USE,PlotModulusA.inp\r\n');
            %fprintf(fid0,'*USE,SimpleBCs.inp\r\n');
            fprintf(fid0,'*USE,SimpleSWFBCs.inp\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'eplot,all\r\n');
            fprintf(fid0,'/SOLU\r\n');
            fprintf(fid0,'SOLVE\r\n');
            fprintf(fid0,'*USE,PostProcess.inp\r\n');
            fclose(fid0);
        case 'B'
            disp(['*****GENERATING RunAnsysModelB.inp*****'])
            disp('*')
            fid0 = fopen('RunAnsysModelB.inp','w');
            fprintf(fid0,'finish\r\n');
            fprintf(fid0,'/CLEAR,ALL\r\n');
            fprintf(fid0,'/PREP7\r\n');
            fprintf(fid0,'*USE,AnsysMeshB.cdb\r\n');
            fprintf(fid0,'*USE,MaterialPropsB.inp\r\n');
            fprintf(fid0,'*USE,PlotModulusB.inp\r\n');
            %fprintf(fid0,'*USE,SimpleBCs.inp\r\n');
            fprintf(fid0,'*USE,SimpleSWFBCs.inp\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'eplot,all\r\n');
            fprintf(fid0,'/SOLU\r\n');
            fprintf(fid0,'SOLVE\r\n');
            fprintf(fid0,'*USE,PostProcess.inp\r\n');
            fclose(fid0);
        case 'C'
            disp(['*****GENERATING RunAnsysModelC.inp*****'])
            disp('*')
            fid0 = fopen('RunAnsysModelC.inp','w');
            fprintf(fid0,'finish\r\n');
            fprintf(fid0,'/CLEAR,ALL\r\n');
            fprintf(fid0,'/PREP7\r\n');
            fprintf(fid0,'*USE,AnsysMeshC.cdb\r\n');
            fprintf(fid0,'*USE,MaterialPropsC.inp\r\n');
            fprintf(fid0,'*USE,NodalTempsC.inp\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'eplot,all\r\n');

            fprintf(fid0,'!**********FIRST LOADING STEP**********!\r\n');
            fprintf(fid0,'!*** WE START by solving an initial step with no load but temperature\r\n');
            fprintf(fid0,'!*** This results in no deformation but assigns the E-modulus to the nodes\r\n');
            fprintf(fid0,'/SOLU\r\n');
            fprintf(fid0,'ANTYPE,0\r\n');
            fprintf(fid0,'TIME,1.0\r\n');
            fprintf(fid0,'NSUBST,1,100,1	!No. of substeps, max, min\r\n');
            fprintf(fid0,'OUTRES,ALL,-res\r\n');
            fprintf(fid0,'RESCONTROL,DEFINE,ALL,LAST	!Write restart file for last substep of each load step\r\n');
            fprintf(fid0,'allsel,all\r\n');

            fprintf(fid0,'!*** SOLVE TEMPERATURE ONLY STEP\r\n');
            fprintf(fid0,'SOLVE\r\n');
            fprintf(fid0,'SAVE,,,,ALL\r\n');
            fprintf(fid0,'PARSAV,ALL,Z_Params,parm		!Save parameters\r\n');
            fprintf(fid0,'FINISH\r\n');

            fprintf(fid0,'!**********SECOND LOADING STEP**********!\r\n');
            fprintf(fid0,'!*** NOW WE START solving the main load case (displacements applied)\r\n');
            fprintf(fid0,'/SOLU\r\n');
            fprintf(fid0,'ANTYPE,0,REST,,,CONTINUE	!Restart at end of previous load step\r\n');
            fprintf(fid0,'PARRES,NEW,Z_Params,parm	!Restore parameters lost from restart\r\n');
            %fprintf(fid0,'*USE,SimpleBCs.inp\r\n');
            fprintf(fid0,'*USE,SimpleSWFBCs.inp\r\n');
            fprintf(fid0,'TIME,2.0\r\n');
            fprintf(fid0,'NSUBST,1,100,1	!No. of substeps, max, min\r\n');
            fprintf(fid0,'OUTRES,ALL,-1		!Write all results to db (1 per load step)\r\n');
            fprintf(fid0,'RESCONTROL,DEFINE,ALL,LAST	! Write restart file for last substep of each load step\r\n');

            fprintf(fid0,'SOLVE\r\n');
            fprintf(fid0,'SAVE,,,,ALL\r\n');
            fprintf(fid0,'FINISH\r\n');
            fprintf(fid0,'!**************************************!\r\n');
            fprintf(fid0,'*USE,PlotModulusC.inp\r\n');
            fprintf(fid0,'*USE,PostProcess.inp\r\n');
            fclose(fid0);
        case 'D'
            disp(['*****GENERATING RunAnsysModelD.inp*****'])
            disp('*')
            fid0 = fopen('RunAnsysModelD.inp','w');
            fprintf(fid0,'finish\r\n');
            fprintf(fid0,'/CLEAR,ALL\r\n');
            fprintf(fid0,'/PREP7\r\n');
            fprintf(fid0,'*USE,AnsysMeshD.cdb\r\n');
            fprintf(fid0,'*USE,MaterialPropsD.inp\r\n');
            fprintf(fid0,'*USE,NodalTempsD.inp\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'eplot,all\r\n');

            fprintf(fid0,'!**********FIRST LOADING STEP**********!\r\n');
            fprintf(fid0,'!*** WE START by solving an initial step with no load but temperature\r\n');
            fprintf(fid0,'!*** This results in no deformation but assigns the E-modulus to the nodes\r\n');
            fprintf(fid0,'/SOLU\r\n');
            fprintf(fid0,'ANTYPE,0\r\n');
            fprintf(fid0,'TIME,1.0\r\n');
            fprintf(fid0,'NSUBST,1,100,1	!No. of substeps, max, min\r\n');
            fprintf(fid0,'OUTRES,ALL,-res\r\n');
            fprintf(fid0,'RESCONTROL,DEFINE,ALL,LAST	!Write restart file for last substep of each load step\r\n');
            fprintf(fid0,'allsel,all\r\n');

            fprintf(fid0,'!*** SOLVE TEMPERATURE ONLY STEP\r\n');
            fprintf(fid0,'SOLVE\r\n');
            fprintf(fid0,'SAVE,,,,ALL\r\n');
            fprintf(fid0,'PARSAV,ALL,Z_Params,parm		!Save parameters\r\n');
            fprintf(fid0,'FINISH\r\n');

            fprintf(fid0,'!**********SECOND LOADING STEP**********!\r\n');
            fprintf(fid0,'!*** NOW WE START solving the main load case (displacements applied)\r\n');
            fprintf(fid0,'/SOLU\r\n');
            fprintf(fid0,'ANTYPE,0,REST,,,CONTINUE	!Restart at end of previous load step\r\n');
            fprintf(fid0,'PARRES,NEW,Z_Params,parm	!Restore parameters lost from restart\r\n');
            %fprintf(fid0,'*USE,SimpleBCs.inp\r\n');
            fprintf(fid0,'*USE,SimpleSWFBCs.inp\r\n');
            fprintf(fid0,'TIME,2.0\r\n');
            fprintf(fid0,'NSUBST,1,100,1	!No. of substeps, max, min\r\n');
            fprintf(fid0,'OUTRES,ALL,-1		!Write all results to db (1 per load step)\r\n');
            fprintf(fid0,'RESCONTROL,DEFINE,ALL,LAST	! Write restart file for last substep of each load step\r\n');

            fprintf(fid0,'SOLVE\r\n');
            fprintf(fid0,'SAVE,,,,ALL\r\n');
            fprintf(fid0,'FINISH\r\n');
            fprintf(fid0,'!**************************************!\r\n');
            fprintf(fid0,'*USE,PlotModulusD.inp\r\n');
            fprintf(fid0,'*USE,PostProcess.inp\r\n');
            fclose(fid0);

        case 'E'
            disp(['*****GENERATING RunAnsysModelE.inp*****'])
            disp('*')
            fid0 = fopen('RunAnsysModelE.inp','w');
            fprintf(fid0,'finish\r\n');
            fprintf(fid0,'/CLEAR,ALL\r\n');
            fprintf(fid0,'/PREP7\r\n');
            fprintf(fid0,'*USE,AnsysMeshE.cdb\r\n');
            fprintf(fid0,'*USE,MaterialPropsE.inp\r\n');
            fprintf(fid0,'*USE,ShellMeshE.cdb\r\n');
            fprintf(fid0,'*USE,ShellMaterialsE.inp\r\n');
            fprintf(fid0,'*USE,PlotModulusE.inp\r\n');
            %fprintf(fid0,'*USE,SimpleBCs.inp\r\n');
            fprintf(fid0,'*USE,SimpleSWFBCs.inp\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'eplot,all\r\n');
            fprintf(fid0,'/SOLU\r\n');
            fprintf(fid0,'SOLVE\r\n');
            fprintf(fid0,'*USE,PostProcess.inp\r\n');
            fclose(fid0);
    end


%******************Writing SimpleBCs.inp************
function WriteSimpleBCs;
    fid0 = fopen('SimpleBCs.inp','w');
    fprintf(fid0,'allsel,all\r\n');
    fprintf(fid0,'Nsel,s,,,1\r\n');
    fprintf(fid0,'D,all,UY,0.1\r\n');
    fprintf(fid0,'Nsel,s,,,2,100\r\n');
    fprintf(fid0,'D,all,all\r\n');
    fprintf(fid0,'allsel,all\r\n');
    fclose(fid0);


%******************Writing SimpleSWFBCs.inp************
function WriteSimpleSWFBCs;
    fid0 = fopen('SimpleSWFBCs.inp','w');
    fprintf(fid0,'allsel,all\r\n');

    fprintf(fid0,'!***SELECTING the support nodes at the distal end***\r\n');
    fprintf(fid0,'NSEL,S,EXT\r\n');
    fprintf(fid0,'*GET,MinShaftZ,NODE,0,MNLOC,Z\r\n');
    fprintf(fid0,'NSEL,R,LOC,Z,MinShaftZ-0.5,MinShaftZ+0.5\r\n');
    fprintf(fid0,'CM,ContNodShaft,NODE\r\n');
    fprintf(fid0,'D,ALL,UZ,0\r\n');
    fprintf(fid0,'allsel,all\r\n');

    fprintf(fid0,'!***SELECTING the support nodes at the femoral head***\r\n');
    fprintf(fid0,'NSEL,S,EXT\r\n');
    fprintf(fid0,'*GET,MinHeadX,NODE,0,MNLOC,X\r\n');
    fprintf(fid0,'NSEL,R,LOC,X,MinHeadX-0.5,MinHeadX+5.0\r\n');
    fprintf(fid0,'CM,ContNodHead,NODE\r\n');
    fprintf(fid0,'D,ALL,UX,0\r\n');
    fprintf(fid0,'allsel,all\r\n');

    fprintf(fid0,'!***SELECTING the support nodes at the greater trochanter***\r\n');
    fprintf(fid0,'NSEL,S,EXT\r\n');
    fprintf(fid0,'*GET,MaxGTX,NODE,0,MXLOC,X\r\n');
    fprintf(fid0,'NSEL,R,LOC,X,MaxGTX-5.0,MaxGTX+0.5\r\n');
    fprintf(fid0,'CM,ContNodGT,NODE\r\n');
    fprintf(fid0,'D,ALL,UX,-1.0\r\n');
    fprintf(fid0,'D,ALL,UY,0.0\r\n');

    fprintf(fid0,'allsel,all\r\n');



%******************Writing PlotModulusX.in************
% X refers to A, B, C, D or E
function WritePlotScript(Method);
    switch Method
        case {'A','B','E'}
            if strcmp(Method,'A')
                disp(['*****GENERATING PlotModulusA.inp*****'])
                disp('*')
                fid0 = fopen('PlotModulusA.inp','w');
            end
            if strcmp(Method,'B')
                disp(['*****GENERATING PlotModulusB.inp*****'])
                disp('*')
                fid0 = fopen('PlotModulusB.inp','w');
            end
            if strcmp(Method,'E')
                disp(['*****GENERATING PlotModulusE.inp*****'])
                disp('*')
                fid0 = fopen('PlotModulusE.inp','w');
            end
            fprintf(fid0,'/PREP7\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'CMSEL,S,GemEls\r\n');
            if strcmp(Method,'E')
               fprintf(fid0,'CMSEL,A,ShellElements\r\n');
            end
            fprintf(fid0,'BFDELE,ALL,ALL\r\n');
            fprintf(fid0,'*get,ne,elem,num,count\r\n');
            fprintf(fid0,'c = 0\r\n');
            fprintf(fid0,'volume_tot = \r\n');
            fprintf(fid0,'volume_tot = 0\r\n');
            fprintf(fid0,'el_t_vol = \r\n');
            fprintf(fid0,'el_t_vol = 0\r\n');
            fprintf(fid0,'E_mod_max = \r\n');
            fprintf(fid0,'E_mod_max = 0\r\n');
            fprintf(fid0,'el_nrs = \r\n');
            fprintf(fid0,'*DIM,el_nrs,array,ne\r\n');
            fprintf(fid0,'E_el = \r\n');
            fprintf(fid0,'*DIM,E_el,array,ne\r\n');
            fprintf(fid0,'*DO,i,1,ne\r\n');
                fprintf(fid0,'c = elnext(c)\r\n');
                fprintf(fid0,'*GET,mat_num,ELEM,c,ATTR,MAT\r\n');
                fprintf(fid0,'*GET,E_mod,EX,mat_num,TEMP,0\r\n');
                fprintf(fid0,'*IF,E_mod,GT,E_mod_max,THEN\r\n');
                    fprintf(fid0,'E_mod_max = E_mod\r\n');
                fprintf(fid0,'*ENDIF\r\n');
                fprintf(fid0,'*GET,el_volume,ELEM,c,VOLU,\r\n');
                fprintf(fid0,'volume_tot = volume_tot+el_volume\r\n');
                fprintf(fid0,'el_t_vol = el_t_vol+el_volume*E_mod\r\n');
                fprintf(fid0,'BFE,c,TEMP,,E_mod\r\n');
                fprintf(fid0,'el_nrs(i) = c\r\n');
                fprintf(fid0,'E_el(i) = E_mod\r\n');
            fprintf(fid0,'*ENDDO\r\n');
            fprintf(fid0,'e_ave_vol = el_t_vol/volume_tot\r\n');
            fprintf(fid0,'*VSCFUN,e_ave,MEAN,E_el\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fclose(fid0);

        case {'C','D'}
            if strcmp(Method,'C')
                disp(['*****GENERATING PlotModulusC.inp*****'])
                disp('*')
                fid0 = fopen('PlotModulusC.inp','w');
            end
            if strcmp(Method,'D')
                disp(['*****GENERATING PlotModulusD.inp*****'])
                disp('*')
                fid0 = fopen('PlotModulusD.inp','w');
            end
            fprintf(fid0,'/POST1\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'set,last\r\n');
            fprintf(fid0,'CMSEL,S,GemEls\r\n');
            fprintf(fid0,'ETABLE,E,BFE,TEMP\r\n');
            fprintf(fid0,'ETABLE,ELEMVOL,VOLU,\r\n');
            fprintf(fid0,'!*  \r\n');
            fprintf(fid0,'*get,ne,elem,num,count\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fprintf(fid0,'el_nrs = \r\n');
            fprintf(fid0,'*DIM,el_nrs,array,ne\r\n');
            fprintf(fid0,'c=0\r\n');
            fprintf(fid0,'*DO,i,1,ne\r\n');
                fprintf(fid0,'c=elnext(c)\r\n');
                fprintf(fid0,'el_nrs(i) = c\r\n');
            fprintf(fid0,'*ENDDO\r\n');

            fprintf(fid0,'E_el = \r\n');
            fprintf(fid0,'el_volume = \r\n');
            fprintf(fid0,'*dim,E_el,array,ne\r\n');
            fprintf(fid0,'*dim,el_volume,array,ne\r\n');
            fprintf(fid0,'*VGET,E_el,ELEM, ,ETAB,E, ,2\r\n');
            fprintf(fid0,'*VGET,el_volume,ELEM, ,ETAB,ELEMVOL, ,2\r\n');
            fprintf(fid0,'*VOPER,dummy,E_el,MULT,el_volume, , ,\r\n');
            fprintf(fid0,'*VSCFUN,el_t_vol,SUM,dummy\r\n');
            fprintf(fid0,'*VSCFUN,volume_tot,SUM,el_volume\r\n');
            fprintf(fid0,'*VSCFUN,E_mod_max,MAX,E_el\r\n');
            fprintf(fid0,'*VSCFUN,E_ave,MEAN,E_el\r\n');
            fprintf(fid0,'E_ave_vol = el_t_vol/volume_tot\r\n');
            fprintf(fid0,'allsel,all\r\n');
            fclose(fid0);
    end



%******************Writing PostProcess.inp************
function WritePostProcess;
    fid0 = fopen('PostProcess.inp','w');
    fprintf(fid0,'!***THIS IS JUST A SAMPLE Post Processing script***\r\n');
    fprintf(fid0,'!***included here for demonstration purposes***\r\n');
    fprintf(fid0,'/POST1\r\n');
    fprintf(fid0,'allsel,all\r\n');

    fprintf(fid0,'!FIND THE LOAD APPLIED at the GT\r\n');
    fprintf(fid0,'CMSEL,S,ContNodGT\r\n');
    fprintf(fid0,'nnn = \r\n');
    fprintf(fid0,'*GET,nnn,NODE,NUM,COUNT\r\n');

    fprintf(fid0,'SET,LAST\r\n');
    fprintf(fid0,'CMSEL,S,ContNodGT,NODE\r\n');
    fprintf(fid0,'C=0\r\n');
    fprintf(fid0,'dum =\r\n');
    fprintf(fid0,'dum = 0\r\n');
    fprintf(fid0,'*DO,j,1,nnn\r\n');
        fprintf(fid0,'C=NDNEXT(C)\r\n');
        fprintf(fid0,'*GET,dumX,NODE,C,RF,FX\r\n');
        fprintf(fid0,'dum = dum + dumX\r\n');
    fprintf(fid0,'*ENDDO\r\n');
    fprintf(fid0,'RForceEP = \r\n');
    fprintf(fid0,'RForceEP = abs(dum)\r\n');

    fprintf(fid0,'IgnoreRad = \r\n');
    fprintf(fid0,'IgnoreRad = 15\r\n');
    fprintf(fid0,'NSEL,S,LOC,Z,MinShaftZ-IgnoreRad,MinShaftZ+IgnoreRad\r\n');
    fprintf(fid0,'CM,UnselShaft,NODE\r\n');
    fprintf(fid0,'NSEL,S,LOC,X,MinHeadX-IgnoreRad,MinHeadX+IgnoreRad\r\n');
    fprintf(fid0,'CM,UnselFH,NODE\r\n');
    fprintf(fid0,'NSEL,S,LOC,X,MaxGTX-IgnoreRad,MaxGTX+IgnoreRad\r\n');
    fprintf(fid0,'CM,UnselGT,NODE\r\n');

    fprintf(fid0,'NSEL,ALL\r\n');
    fprintf(fid0,'NSEL,U,,,UnselShaft\r\n');
    fprintf(fid0,'NSEL,U,,,UnselFH\r\n');
    fprintf(fid0,'NSEL,U,,,UnselGT\r\n');
    fprintf(fid0,'CM,PostProNodes,NODE\r\n');

    fprintf(fid0,'!WE output the results of the last time step\r\n');
    fprintf(fid0,'SET,LAST\r\n');
    fprintf(fid0,'ESEL,S,ENAME,,187\r\n');

    fprintf(fid0,'*GET,NE_bein,ELEM,NUM,COUNT\r\n');
    fprintf(fid0,'ce =\r\n');
    fprintf(fid0,'ce = 0\r\n');
    fprintf(fid0,'ce = elnext(ce)\r\n');
    fprintf(fid0,'pp = NELEM(ce,1)\r\n');
    fprintf(fid0,'NSEL,S,,,pp\r\n');
    fprintf(fid0,'*DO,i_bein,1,NE_bein\r\n');
        fprintf(fid0,'p1 = NELEM(ce,1)\r\n');
        fprintf(fid0,'p2 = NELEM(ce,2)\r\n');
        fprintf(fid0,'p3 = NELEM(ce,3)\r\n');
        fprintf(fid0,'p4 = NELEM(ce,4)\r\n');
        fprintf(fid0,'NSEL,A,,,p1\r\n');
        fprintf(fid0,'NSEL,A,,,p2\r\n');
        fprintf(fid0,'NSEL,A,,,p3\r\n');
        fprintf(fid0,'NSEL,A,,,p4\r\n');
        fprintf(fid0,'ce = elnext(ce)\r\n');
    fprintf(fid0,'*ENDDO\r\n');
    fprintf(fid0,'!ALL cornernodes in the bone elements\r\n');
    fprintf(fid0,'CM,horn_pkt,NODE\r\n');

    fprintf(fid0,'!ALL external corner nodes\r\n');
    fprintf(fid0,'NSEL,S,EXT\r\n');
    fprintf(fid0,'NSEL,R,,,horn_pkt\r\n');
    fprintf(fid0,'CM,ext_horn_pkt,NODE\r\n');
    fprintf(fid0,'NSEL,U,,,UnselShaft\r\n');
    fprintf(fid0,'NSEL,U,,,UnselFH\r\n');
    fprintf(fid0,'NSEL,U,,,UnselGT\r\n');
    fprintf(fid0,'CM,ext_horn_pkt_cropped,NODE\r\n');

    fprintf(fid0,'NSEL,S,,,ext_horn_pkt_cropped\r\n');
    fprintf(fid0,'*GET,n_ext,NODE,NUM,COUNT\r\n');
    fprintf(fid0,'e_ext_out =\r\n');
    fprintf(fid0,'*DIM,e_ext_out1,ARRAY,n_ext\r\n');
    fprintf(fid0,'*DIM,e_ext_out3,ARRAY,n_ext\r\n');
    fprintf(fid0,'*DIM,NodeNr,ARRAY,n_ext\r\n');
    fprintf(fid0,'c_ext = 0\r\n');
    fprintf(fid0,'*DO,i_ext,1,n_ext\r\n');
    fprintf(fid0,'c_ext = NDNEXT(c_ext)\r\n');
    fprintf(fid0,'NodeNr(i_ext) = c_ext\r\n');
    fprintf(fid0,'*GET,dum1,node,c_ext,epto,1\r\n');
    fprintf(fid0,'*GET,dum3,node,c_ext,epto,3\r\n');
    fprintf(fid0,'e_ext_out1(i_ext) = dum1\r\n');
    fprintf(fid0,'e_ext_out3(i_ext) = dum3\r\n');
    fprintf(fid0,'*ENDDO\r\n');

    fprintf(fid0,'*VSCFUN,maxEP1,MAX,e_ext_out1\r\n');
    fprintf(fid0,'*VSCFUN,LOCmaxEP1,LMAX,e_ext_out1\r\n');
    fprintf(fid0,'NodeMaxStrain = e_ext_out1(LOCmaxEP1)\r\n');
    fprintf(fid0,'*VSCFUN,minEP3,MIN,e_ext_out3\r\n');
    fprintf(fid0,'*VSCFUN,LOCminEP3,LMIN,e_ext_out3\r\n');
    fprintf(fid0,'NodeMinStrain = e_ext_out3(LOCminEP3)\r\n');

    fprintf(fid0,'WholeBoneStrength1 = \r\n');
    fprintf(fid0,'WholeBoneStrength3 = \r\n');
    fprintf(fid0,'WholeBoneStrength1 = RForceEP*0.0073/abs(maxEP1)\r\n');
    fprintf(fid0,'WholeBoneStrength3 = RForceEP*0.0104/abs(minEP3)\r\n');
    fprintf(fid0,'*IF,WholeBoneStrength1,GT,WholeBoneStrength3,THEN \r\n');
        fprintf(fid0,'WholeBoneStrength = WholeBoneStrength3\r\n');
    fprintf(fid0,'*ELSE\r\n');
        fprintf(fid0,'WholeBoneStrength = WholeBoneStrength1\r\n');
    fprintf(fid0,'*ENDIF\r\n');
    fprintf(fid0,'/EFACET,1\r\n');
    fprintf(fid0,'/EDGE,1,0,45\r\n');
    fprintf(fid0,'/GLINE,1,-1\r\n');
    fprintf(fid0,'PLNSOL, EPTO,3, 0,1.0\r\n');
