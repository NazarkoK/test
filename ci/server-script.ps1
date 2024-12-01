param (
    [string]$port,
    [string]$portSpeed
)

$repoDir = (Get-Item -Path $PSScriptRoot).Parent.FullName
$binDir = "$repoDir\bin"
$testResultsDir = "$repoDir\ci\result_test"
$hexOutputDir = "$repoDir\ci\build\"
$arduinoCliPath = "$binDir\arduino-cli.exe"
$sketchPath = "$repoDir\src\server\server.ino"
$outputHexPath = "$hexOutputDir\server-script-arduino-uno-r3.ino.hex"

if (-not (Test-Path $arduinoCliPath)) {
    Write-Host "Arduino CLI not found, proceeding with installation..."
    New-Item -ItemType Directory -Force -Path $binDir | Out-Null
    Invoke-WebRequest -Uri "https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip" -OutFile "$binDir\arduino-cli.zip"
    Expand-Archive -Path "$binDir\arduino-cli.zip" -DestinationPath $binDir -Force
    Remove-Item "$binDir\arduino-cli.zip"
} else {
    Write-Host "Arduino CLI is already installed."
}

Write-Host "Configuring Arduino CLI now..."
& $arduinoCliPath config init
& $arduinoCliPath core update-index
& $arduinoCliPath core install arduino:avr

Write-Host "Compiling Arduino sketch..."
New-Item -ItemType Directory -Force -Path $hexOutputDir | Out-Null
& $arduinoCliPath compile --fqbn arduino:avr:uno --output-dir $hexOutputDir $sketchPath

Write-Host "Saving compiled HEX file..."
Copy-Item -Path "$hexOutputDir\server.ino.hex" -Destination $outputHexPath -Force

if ($port) {
    Write-Host "Uploading HEX file to Arduino on port $port with speed $portSpeed..."
    & $arduinoCliPath upload -p $port --fqbn arduino:avr:uno -i $outputHexPath

}

else {
    Write-Host "No COM port provided. Skipping upload and hardware tests."
}
