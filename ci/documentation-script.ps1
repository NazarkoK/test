$repoDir = (Get-Item -Path $PSScriptRoot).Parent.FullName
$binDir = "$repoDir\bin"
$srcDir = "$repoDir\src"
$testsDir = "$repoDir\tests\test_server\test_server"
$documentationDir = "$repoDir\ci\documentation"
$latexDir = "$documentationDir\latex"
$xmlDir = "$documentationDir\xml"
$htmlDir = "$documentationDir\html"
$dirsToCreate = @($binDir, $documentationDir, $xmlDir, $htmlDir, $latexDir, $testsDir)
foreach ($dir in $dirsToCreate) {
    if (-not (Test-Path $dir)) {
        Write-Host "Creating directory: $dir"
        New-Item -ItemType Directory -Path $dir | Out-Null
    }
}
$doxygenUrl = "https://github.com/doxygen/doxygen/releases/download/Release_1_12_0/doxygen-1.12.0.windows.x64.bin.zip"
$doxygenZipFile = "$binDir\doxygen.zip"
$doxygenDir = "$binDir\doxy"
$doxygenExeFile = "$doxygenDir\doxygen.exe"
if (-not (Test-Path $doxygenExeFile)) {
    Write-Host "Doxygen not found, downloading..."
    Invoke-WebRequest -Uri $doxygenUrl -OutFile $doxygenZipFile
    Write-Host "Downloaded Doxygen to $doxygenZipFile"
    Expand-Archive -Path $doxygenZipFile -DestinationPath $doxygenDir -Force
    Write-Host "Expanded Doxygen to $doxygenDir"
} else {
    Write-Host "Doxygen already installed at $doxygenDir"
}
$env:Path += ";$doxygenDir"
$doxygenConfigFile = "$documentationDir\Doxyfile"
if (-not (Test-Path $doxygenConfigFile)) {
    & $doxygenExeFile -g $doxygenConfigFile
}
$projectName = "csad2425KI405KachurNazar11"
(Get-Content $doxygenConfigFile) -replace 'PROJECT_NAME\s*=\s*\".*\"', "PROJECT_NAME = `"$projectName`"" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'INPUT\s*=\s*.*', "INPUT = $srcDir $testsDir" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace "FILE_PATTERNS\s*=\s*.*", "FILE_PATTERNS = *.cpp *.h *.ino" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'RECURSIVE\s*=\s*.*', "RECURSIVE = YES" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'EXCLUDE_PATTERNS\s*=\s*.*', "EXCLUDE_PATTERNS = *AssemblyAttributes.cpp" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'GENERATE_XML\s*=\s*NO', "GENERATE_XML = YES" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'XML_OUTPUT\s*=\s*.*', "XML_OUTPUT = $xmlDir" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'GENERATE_HTML\s*=\s*NO', "GENERATE_HTML = YES" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'HTML_OUTPUT\s*=\s*.*', "HTML_OUTPUT = $htmlDir" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'LATEX_OUTPUT\s*=\s*.*', "LATEX_OUTPUT = $latexDir" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'EXTRACT_ALL\s*=\s*NO', "EXTRACT_ALL = YES" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'EXTRACT_PRIVATE\s*=\s*NO', "EXTRACT_PRIVATE = YES" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'EXTRACT_STATIC\s*=\s*NO', "EXTRACT_STATIC = YES" | Set-Content $doxygenConfigFile
(Get-Content $doxygenConfigFile) -replace 'EXTENSION_MAPPING\s*=\s*.*', 'EXTENSION_MAPPING = ino=C++' | Set-Content $doxygenConfigFile
& $doxygenExeFile $doxygenConfigFile
Write-Host "Documentation generated in $documentationDir"
