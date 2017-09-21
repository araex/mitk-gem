%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% WriteAbaqusInput.m, vers. 1, 21.09.2017
% Author: Ifaz Haider, UCalgary, Canada 
% Signficant portions of this code were adopted from 'writeAnsysInput.m'
% written by Benedikt Helgason and ETH-Zurich, Switzerland.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% DESCRIPTION AND QUICK START GUIDE
%
% This function converts the material mapped mesh, generated from MITK-GEM
% into an input file which can be imported into ABAQUS
%
% The function was tested on femur models generated from clinical CT data and imported into ABAQUS CAE 2016.
% It may not be robust for other models or different ABAQUS versions.
%
% A tutorial describing the segmentation pipeline is provided below:
% http://araex.github.io/mitk-gem-site/#pipeline
%
% Following this example, an ABAQUS input file can be generated using the
% following parameters:
% WriteAbaqusInput(['material mapped mesh.txt'],'A',500,[],[],[])
% WriteAbaqusInput(['material mapped mesh.txt'],'B',500,[],[],[])
%
%OUTPUT
%
%This script generates 3 separate files which must be in the same folder
%for ABAQUS to import correctly. 
%
%abaqus_nodes.txt - nodal numbers and coordinates
%abaqus_elements.txt - element connectivity map
%abaqus_model.inp - 'main' model file interpreted by ABAQUS. Points to the
%previous 2 node and element files and contains material information.
%
%To import into ABAQUS CAE: file->import->import model->point to 'abaqus_model.inp'
%This will import a part with the correct geoemetry and material
%properties.The user must specify loads and BCs within ABAQUS CAE in order to run a
%model. The process is identical to any other common FE task.
%
%Future releases of this script may incoporate simple BC cases
%automatically. Regardless, users are highly encouraged to use the ABAQUS CAE
%interface to confirm loading sites and directions.
%
%
%INPUT
% 
% The function takes several inputs:
% writeAbaqusInput(GEMfilename,Method,NumOfMats,Ecort,EminShell,ShellTh)
%
%GEMfilename: the complete path and filename for the text file with the
%model data that MITK-GEM outputs.
%
%Method: input 'A', 'B', 'C', 'D' and 'E' where A-E refers to the material
%mapping method in Helgason et al. 2016, Med Eng and Phys 38:679-689.
%************ONLY METHODS A AND B ARE IMPLEMENTED FOR ABAQUs EXPORT******
%*************Methods C D and E are planned for a future update**********
%
%NumOfMats: Elements with similar material properties are grouped together
%and assigned an average stiffness. This is the number of groups that will
%be generated. This value must be less than the number of elements in the
%model. Typically a value of 500 works well for femur models.
%
%Ecort: Refers to the cortical bone E-modulus used in method D. Please
%leave blank i.e., '[]' until method D is implemented. 
%
%EminShell: Refers to the minimum E-modulus used for shell elements when
%method E is used. In Bessho et al. 2007, JBM 40:1745-1753 this value
%was set to 10 GPa.Please leave blank i.e., '[]' until method E is implemented. 
%
%ShellTh: The shell thickness used for the shell elements when method E is
%used. In Bessho et al. 2007, JBM 40:1745-1753 this value was set to
%0.4 mm which was equal to the pixel width of their CT data (0.398 mm). 
%Please leave blank i.e., '[]' until method E is implemented. 
%
%ADDITIONAL COMMENTS
%
%
%
function writeAbaqusInput(GEMfilename,Method,NumOfMats,Ecort,EminShell,ShellTh)

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

%intialize an object which contains elements sorted by mat props
El_Mat.matNum=[];
El_Mat.E=[];
El_Mat.ElsInMat={};

% ******************PLACING MATARIALS INTO NumOfMats BINS*****************
switch Method
    case 'A'
        %Write the Method A material file
        El_E = ELEMENTS(:,12);
        El_Mat = SortElsByMat(El_E,NumOfMats,'A',EminShell,Ecort,ShellTh,SURFACES)
        %El_Mat = WriteMatFile(El_E,NumOfMats,'A',EminShell,Ecort,ShellTh,SURFACES);
    case 'B'
        %Write the Method B material file
        El_E = ELEMENTS(:,13);
        El_Mat = SortElsByMat(El_E,NumOfMats,'B',EminShell,Ecort,ShellTh,SURFACES)
        %El_Mat = WriteMatFile(El_E,NumOfMats,'B',EminShell,Ecort,ShellTh,SURFACES);
    case 'C'
        %Write the Method C material file
        fprintf('*****Method C not implemented in ABAQUS*****\n')
        WriteMatFile([],NumOfMats,'C',EminShell,Ecort,ShellTh,SURFACES,NODES);
        %El_Mat = ones(length(ELEMENTS(:,1)),1);
    case 'D'
        fprintf('*****Method D not implemented in ABAQUS*****\n')
        %Write the Method D material file
        %WriteMatFile([],NumOfMats,'D',EminShell,Ecort,ShellTh,SURFACES,NODES);
        %El_Mat = ones(length(ELEMENTS(:,1)),1);
    case 'E'
        fprintf('*****Method E is planned for a future release of this script*****\n')
%         %Write the Method E material file
%         El_E = ELEMENTS(:,12);
% 
%         if (isempty(EminShell))
%             error('*****Min Cortical Young''s modulus needs to greater than 0*****')
%         end
%         if (EminShell <= 0)
%             error('*****Min Cortical Young''s modulus needs to greater than 0*****')
%         end
%         if isempty(ShellTh)
%             error('*****Shell thickness needs to be larger than 0 for method E*****')
%         end
%         if ShellTh <= 0
%             error('*****Shell thickness needs to be larger than 0 for method E*****')
%         end
%         [El_Mat,E_Mat] = WriteMatFile(El_E,NumOfMats,'E',EminShell,Ecort,ShellTh,SURFACES,NODES);
%         WriteShellFile(EminShell,ShellTh,NumOfMats,El_Mat,E_Mat,SURFACES,NODES)
end

%now that we have nodes, elements, and bins, we can start to write the inp
%file. This simply follows ABAQUS input file conventions:

%write header info, define nodes and elements
fprintf('*****INPUT FILE: %s*****\n',FileName)
fprintf('*\n')
fidwrite=fopen('abaqus_model.inp','w');

fprintf(fidwrite,'**ABAQUS inputfile generated by writeAbaqusInput.m \n');
fprintf(fidwrite,'**This file was created on %s\r\n\r\n',datestr(clock));
fprintf(fidwrite,'*Preprint, echo=NO, model=NO, history=NO, contact=NO\n');

fprintf(fidwrite,'*Part, name=Part1\n');
fprintf(fidwrite,'*Node, input=abaqus_nodes.txt\n');
fprintf(fidwrite,'*Element, type=C3D10, input=abaqus_elements.txt\n');

%write element sets; each set defines a collection of elemements which will be assigned a single
%material property in future sections of the inp file.
for k=1:max(size(El_Mat.ElsInMat))
    if sum(size(El_Mat.ElsInMat{k}))>0
        
        fprintf(fidwrite,'%s%s%s\n','*Elset, elset=ElSetMat', num2str(k), '');
        curEls=El_Mat.ElsInMat{k};
        
        %ABAQUS will only allow 15 elements on a single line. The following
        %loop will write 15 elements and start a new line, repeating until
        %all elements in the set are recorded.
        for m=1:max(size(curEls))
            fprintf(fidwrite,'%s,', num2str(curEls(m)));
            
            if mod(m,15)==0 && max(size(curEls))>m
                fprintf(fidwrite, '\n');
            end
            
        end
        fprintf(fidwrite,'\n');
    end
    
end

%Write Sections. These tell ABAQUS that each material set, defined above,
%is associated with it's own material property.
for k=1:max(size(El_Mat.ElsInMat))
    if sum(size(El_Mat.ElsInMat{k}))>0
        
        fprintf(fidwrite,'%s%s%s%s\n','*Solid Section, elset=ElSetMat', num2str(k), ', material=Mat',num2str(k) );
        fprintf(fidwrite,',\n');
    end
    
end

%end part definition. Create 1 instance of this part for the assembly.
fprintf(fidwrite,'*End Part\n');
fprintf(fidwrite,'**\n');
fprintf(fidwrite,'*Assembly, name=Assembly\n');
fprintf(fidwrite,'**\n');
fprintf(fidwrite,'*Instance, name=Inst1, part=Part1\n');
fprintf(fidwrite,'*End Instance\n');
fprintf(fidwrite,'**\n');
fprintf(fidwrite,'*End Assembly\n');
fprintf(fidwrite,'**\n');

%Finally, define the stiffness and poisson ration to be assigned to each
%section 
fprintf(fidwrite,'**MATERIALS\n');

for k=1:max(size(El_Mat.ElsInMat))
    if sum(size(El_Mat.ElsInMat{k}))>0
        
        fprintf(fidwrite,'%s%s\n','*Material, name=Mat',num2str(k) );
        fprintf(fidwrite,'*Elastic\n');
        fprintf(fidwrite,'%s,%s\n',num2str(El_Mat.E(k)),'0.3' );
        
        %fprintf(fidwrite,',\n');
    end
    
end

dlmwrite('abaqus_nodes.txt', NODES, 'precision', 10);
dlmwrite('abaqus_elements.txt', ELEMENTS(:,1:11), 'precision', 10);

fprintf(fidwrite,'\n');


fclose('all');
end

function [El_Mat] = SortElsByMat(El_E,NumOfMats,Method,EminShell,Ecort,ShellTh,SURFACES)
%seperates all materials into a user specified number of groups
%each group has a width, or bin size, dependent on the range of young's
%modulus in the model as well as the desired number of materials.

%find min and max of modulus values
minE=min(min(El_E));
maxE=max(max(El_E));
%how big is each grouping of elements?
binSize= round( (maxE-minE)/NumOfMats );

%loop through and identify all elements in each bin
for i=1:NumOfMats
    binstart=minE+(i-1)*binSize;
    binend=minE+i*binSize;
    El_Mat.matNum(i)=i;
    El_Mat.E(i)= mean( [binstart binend] );
    
    elsinbin= (El_E >= binstart) & (El_E < binend);
    elsinbin_nums=[];
    for j=1:max( size(elsinbin) )
        if elsinbin(j)==1
          elsinbin_nums=[elsinbin_nums; j];
        end    
    end
    El_Mat.ElsInMat{i}=elsinbin_nums;
    
end

end