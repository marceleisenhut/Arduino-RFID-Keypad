

function ReadSerialLine()
{
    $input = $null
    try
    {
        Write-Host "Reading: " -NoNewline
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
        Write-Host "Writting to Serial Port: " $String
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

    if($AuthEntry.RFID -eq $CardNumber -and $AuthEntry.PIN -eq $PIN -and $AuthEntry.Access -eq $true)
    {
        return $true
    }
    else
    {
        return $false
    }
}

# Create Serial Port Object
$port = New-Object System.IO.Ports.SerialPort
$port.PortName = "COM5"
$port.BaudRate = "9600"
$port.Parity = "None"
$port.DataBits = 8
$port.StopBits = 1
$port.ReadTimeout = 1000000 # 9 seconds
$port.DtrEnable = "true"

# Open Serial Port
Write-Host "Opening Serial Connection on: " $ComPort
$port.open() 
# wait 2 seconds until Arduino is ready
Start-Sleep 2 

# Read Authentification Database
$AuthDB = import-csv -Delimiter ";" -Path .\aut_database.csv 

while($true)
{
    $authTagString = ReadSerialLine 
    Write-Host "AuthString:" $authTagString
    if($authTagString -ne $null)
    {
        $authArray = $authTagString.Replace("`n","").Replace("`r","").split("/")

        switch ($authArray[0])
        {
            "RFID"{$result = AuthentificateCard -CardNumber $authArray[1]}
            "PIN"{$result = AuthetificateCardAndPin -PIN $authArray[1] -CardNumber $authArray[2]  }
        }

        
        switch($result)
        {
            $true {writeSerialLine -String "0"}
            $false{writeSerialLine -String "1"}
            default {}
        }
        
        write-host "Result: "$result

    }



    # CleanUp
    $authTagString = $null
    $authArray = $null
    $result = $null
    sleep 1
    <#
    if ([console]::KeyAvailable)
    {
        $key = [system.console]::readkey($true)
        if (($key.modifiers -band [consolemodifiers]"control") -and ($key.key -eq "C"))
        {
            break
        }
    }#>
}


$port.Close() #closes serial connection

