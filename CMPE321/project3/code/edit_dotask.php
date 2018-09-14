<html>
<head>
</head>
<body>

    <?php
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "cmpe321hw3";

        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);

        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }else{

            // Fetch the record
            $sql = "SELECT TaskID, EmployeeID FROM dotask WHERE TaskID = " . $_GET['taskid'] . " AND EmployeeID = " . $_GET['employeeid'];
            $result = $conn->query($sql);

            session_start();
            $allowedTaskID = "SELECT ID as TaskID FROM task WHERE ProjectID IN (SELECT ProjectID FROM manageproject WHERE ProjectManagerID = " . $_SESSION['ID']. ")";
            $allowedTaskIDResult = $conn->query($allowedTaskID);

            if ($allowedTaskIDResult->num_rows > 0) {
                // If the record actually exists
                if ($result->num_rows > 0) {
                    ?>
                    <form action="edit_dotask_result.php" method="post">

                    <?php

                    // Get the data
                    $row = $result->fetch_assoc();
                    ?>
                        <p>Task ID: 
                        <?php 
                            $select= '<select name="NewTaskID">';
                            while($row2 = $allowedTaskIDResult->fetch_assoc()){
                                if($_GET['taskid']==$row2['TaskID']){
                                    $select.='<option value="'.$row2['TaskID'].'" selected>'.$row2['TaskID'].'</option>';
                                }else{
                                    $select.='<option value="'.$row2['TaskID'].'">'.$row2['TaskID'].'</option>';
                                }
                            }
                            $select.='</select>';
                            echo $select;
                        ?>
                        </p>

                        <p>Employee ID: <input type="text" name="NewEmployeeID" value = "<?php echo $row["EmployeeID"] ?>" /></p>
                        <input type="hidden" name="TaskID" value="<?php echo $_GET['taskid'] ?>">
                        <input type="hidden" name="EmployeeID" value="<?php echo $_GET['employeeid'] ?>">
                        <p><input type="submit" value = "Save Changes" /></p>
                    </form>
                    <?php
                } else {
                    echo "Record does not exist";
                }
            } else {
                echo "You do not have any projects assigned to you.";
            }            
        }
        $conn->close();
    ?>

</body>
</html>
