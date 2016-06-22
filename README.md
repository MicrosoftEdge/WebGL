# Microsoft Edge WebGL Implementation
This repo contains a subset of files from the implementation of [WebGL](https://www.khronos.org/webgl/) that ships in the [Microsoft Edge browser](https://www.microsoft.com/en-us/windows/microsoft-edge). 
The source is provided together with tests, as a reference for engineers who are creating web browsers and related applications. 
By publishing this code, we hope to facilitate the investigation and resolution of any interoperability issues that may exist between different implementations of the standard.

## Folders Content
### Core, Foundation
Header files for common data structures used throughout the code

### GLSLparse
GLSL to HLSL transpiler code 

### ft_glslparse
Test suite for the transpiler

## How do I build this?
At this time, we are publishing the source code for reference only. We do plan to provide project files and instructions to generate binaries down the line, 
however we do not have a target date for it just yet. If you have specific questions or needs, please do reach out: we will be happy to evaluate your scenario and discuss how we can help.     

## Code of Conduct
This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
