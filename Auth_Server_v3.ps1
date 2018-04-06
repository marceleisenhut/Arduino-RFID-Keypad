<#
.Synopsis
   Starts Authentification Server for Arduino PIN and RFID Access Control
.DESCRIPTION
    This Scripts Authentificates RFID + PINs which are send from an Arduino Serial Connection against a csv database.
    Stop the Server with [Ctrl] + [Q]
.EXAMPLE
   .\Auth_Server_v3.ps1 -ComPort "COM9" -BaudRate "9600" -Database .\aut_database.csv
.PARAMETER $ComPort
    COMPort on Which the Arduino is connected
.PARAMETER $BaudRate
    BaudRate for the Serial Connection
.PARAMETER $Database
    Path to the CSV Database File
.NOTES
    Author:    'Stefan Bosser'
    Version:   1.0 / 06.04.2018 /
        - initial
#>

#-- Script Paramter ------------------------------------------------------------------------------------------------------------------#
Param(
    [Parameter(mandatory=$True)]
    [string]$ComPort,

    [Parameter(mandatory=$false)]
    [string]$BaudRate = 9600,

    [Parameter(mandatory=$true)]
    [string]$Database   
)

#-- region functions -----------------------------------------------------------------------------------------------------------------#
function ReadSerialLine()
{
    $input = $null
    try
    {
        Write-Host "Reading                     : " -NoNewline
        do
        {
            $input = $port.ReadLine()
            Write-Host $input
        }while($input -eq "" -or $input -eq $null)
    }

    catch [TimeoutException]
    {
        Write-Error "Serial Read Timeout"
    }
    finally
    {
    } 

    return $input
}

function writeSerialLine($String)
{
    try
    {
        Write-Host "Writting to Serial Port     :" $String
        $port.Write($String)
    }

    catch [TimeoutException]
    {
        Write-Error "Serial Write Error"
    }
    finally
    {
    } 

    return $input
}

function AuthentificateCard($CardNumber)
{
    $AuthEntry = $authdb | Where-Object{$_.RFID -eq $CardNumber}

    if($AuthEntry.RFID -eq $CardNumber -and $AuthEntry.Access -eq $true)
    {
        return $true
    }
    else
    {
        return $false
    }
    
}

function AuthetificateCardAndPin($CardNumber, $PIN)
{
    $AuthEntry = $authdb | Where-Object{$_.RFID -eq $CardNumber}

    if($AuthEntry.RFID -eq $CardNumber -and $AuthEntry.PIN -eq $pin -and $AuthEntry.Access -eq $true)
    {
        return $true
    }
    else
    {
        return $false
    }
}
#-- endregion functions --------------------------------------------------------------------------------------------------------------#

#-- region main ----------------------------------------------------------------------------------------------------------------------#

# Create Serial Port Object
$port = New-Object System.IO.Ports.SerialPort
$port.PortName =$ComPort
$port.BaudRate = $BaudRate
$port.Parity = "None"
$port.DataBits = 8
$port.StopBits = 1
$port.ReadTimeout = 1000000 # 9 seconds
$port.DtrEnable = "true"

try
{# Open Serial Port
    Write-Host "Opening Serial Connection on:" $ComPort
    $port.open() 
    # wait 2 seconds until Arduino is ready
    Start-Sleep 2 
}
catch
{
   Write-Host "Failed to open Port, please try again" -ForegroundColor Red
   Write-host "Error: " $Error[0].FullyQualifiedErrorId -ForegroundColor Red
   sleep 5
   exit
}
# Read Authentification Database

$AuthDB = Import-Csv -Delimiter ";" -Path $Database

while($true)
{
    $key = if ($host.UI.RawUI.KeyAvailable) 
        {
            $host.UI.RawUI.ReadKey('NoEcho, IncludeKeyDown')
        }
    
    $authTagString = ReadSerialLine 
    Write-Host "AuthString                  :" $authTagString
    if($authTagString -ne $null)
    {
        $authArray = $authTagString.Replace("`n","").Replace("`r","").split("/")

        switch ($authArray[0])
        {
            "RFID"{$result = AuthentificateCard -CardNumber $authArray[1]}
            "PIN"{$result = AuthetificateCardAndPin -PIN $authArray[1] -CardNumber $authArray[2]}
        }

        
        switch($result)
        {
            $true {writeSerialLine -String "0"}
            $false{writeSerialLine -String "1"}
            default {}
        }
       
        write-host "Result                      :" $result

    }



    # CleanUp
    $authTagString = $null
    $authArray = $null
    $result = $null
    sleep 1
    
    if ([console]::KeyAvailable)
    {
        $key = $host.UI.RawUI.ReadKey('NoEcho, IncludeKeyDown')
        if (($key.VirtualKeyCode -eq 81) -AND ($key.ControlKeyState -match "LeftCtrlPressed"))
        {
            Write-Host "Stopping Server..."
            break
        }
    }
} 

#closes serial connection
$port.Close() 

#-- endregion main -------------------------------------------------------------------------------------------------------------------#
