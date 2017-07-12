import java.util.Hashtable;
import java.util.Scanner;

public class Main {

	public static void main(String[] args) {
		
		//3 hidden units
		float w1[][] = {{0.01f,  -0.04f,0.04f,-0.05f,0.01f,0.03f,0.06f,0.03f,-0.02f }, // first one is w0
						{0.01f,  -0.03f,0.02f,0.04f,-0.02f,-0.04f,-0.02f,0.03f,0.05f },
						{0.01f,  0.04f,0.03f,0.01f,0.02f,0.02f,-0.01f,-0.04f,-0.03f }};
		
		float w2[][] = { {0.01f, 0.01f,0.02f,0.03f}, {0.01f, 0.03f,0.01f,0.02f}, {0.01f, 0.02f,0.03f,0.01f}, 
						 {0.01f, 0.01f,0.02f,0.01f}, {0.01f, 0.01f,0.03f,0.01f}, {0.01f, 0.04f,0.02f,0.03f}, 
						 {0.01f, 0.03f,0.01f,0.02f}, {0.01f, 0.03f,0.02f,0.01f}};
		
		// 4 hidden units
		float w3[][] = {{0.01f,  -0.04f,0.04f,-0.05f,0.01f,0.03f,0.06f,0.03f,-0.02f }, // first one is w0
						{0.01f,  -0.03f,0.02f,0.04f,-0.02f,-0.04f,-0.02f,0.03f,0.05f },
						{0.01f,  0.04f,0.03f,0.01f,0.02f,0.02f,-0.01f,-0.04f,-0.03f },
						{0.01f,  0.02f,0.01f,0.03f,-0.02f,0.02f,-0.02f,-0.03f,0.01f }};		
		float w4[][] = { {0.01f, 0.01f,0.02f,0.03f,0.01f}, {0.01f, 0.03f,0.01f,0.02f,0.03f}, {0.01f, 0.02f,0.03f,0.01f,0.01f}, 
				 		 {0.01f, 0.01f,0.02f,0.01f,0.02f}, {0.01f, 0.01f,0.03f,0.01f,0.01f}, {0.01f, 0.04f,0.02f,0.03f,0.02f}, 
				 		 {0.01f, 0.03f,0.01f,0.02f,0.02f}, {0.01f, 0.03f,0.02f,0.01f,0.02f}};
		

		float w5[][] = {{0.01f,  -0.04f,0.04f,-0.05f,0.01f,0.03f,0.06f,0.03f,-0.02f }}; // first one is w0
		
		float w6[][] = { {0.01f, 0.01f}, {0.01f, 0.03f}, {0.01f, 0.02f}, 
		 		 {0.01f, 0.01f}, {0.01f, 0.01f}, {0.01f, 0.04f},
		 		 {0.01f, 0.03f}, {0.01f, 0.03f}};
		
		//3 hidden units -- tennis
		/*float w7[][] = {{0.01f,  -0.04f,0.04f,-0.05f,0.01f,0.03f,0.05f,0.03f,-0.02f}, // first one is w0
						{0.01f,  -0.03f,0.02f,0.04f,-0.02f,-0.04f,-0.02f,0.03f,0.05f},
						{0.01f,  0.04f,0.03f,0.01f,0.02f,0.02f,-0.01f,-0.04f,-0.03f},
						{0.01f,  0.04f,0.01f,0.01f,0.03f,-0.02f,-0.01f,-0.04f,0.03f}
					   };*/
		float w7[][] = {{0.01f,  -0.04f,0.04f,-0.05f,0.01f}, // first one is w0
				{0.01f,  -0.03f,0.02f,0.04f,-0.02f},
				{0.01f,  0.04f,0.03f,0.01f,0.02f}};
		//tennis one output
		float w8[][] = { {0.01f, 0.01f,0.02f,0.03f}};

		float w9[][] = {{0.01f,  -0.04f,0.04f,-0.05f,0.01f}, // first one is w0
				{0.01f,  -0.03f,0.02f,0.04f,-0.02f},
				{0.01f,  0.04f,0.03f,0.01f,0.02f},
				{0.01f,  0.04f,0.03f,0.01f,0.02f},
				{0.01f,  0.04f,0.03f,0.01f,0.02f},
				{0.01f,  0.04f,0.03f,0.01f,0.02f}};
		//tennis one output
		float w10[][] = { {0.01f, 0.01f,0.02f,0.03f,0.01f,0.03f,0.01f},
				{0.01f, 0.01f,0.02f,0.03f,0.02f,0.03f,0.02f},
				{0.01f, 0.01f,0.02f,0.03f,0.03f,0.03f,0.02f}};
				
		// read files
		String fn_id_attr = "identity-attr.txt";
		String fn_id_train = "identity-train.txt";
		String fn_ten_attr = "tennis-attr.txt";
		String fn_ten_train = "tennis-train.txt";
		String fn_ten_test = "tennis-test.txt";
		String fn_iris_train = "iris-train.txt";
		String fn_iris_test = "iris-test.txt";
		
		FileOperation fileOper = new FileOperation();
		int attr_amount[] = null;
		Hashtable<String,String> ht = new Hashtable<String,String>();
		
		Scanner scanner=new Scanner(System.in);
		boolean runningFlag = true;
		while(runningFlag) 
		{
			System.out.print("Please input a number to learning and test! \n");
			System.out.print("After executing one task, please re-run the program again.\n");
			System.out.print("1: Identity (3 hidden units) \n");
			System.out.print("2: Identity (4 hidden units) \n");
			System.out.print("3: Tennis  (traing data accuracy)\n");
			System.out.print("4: Tennis  (Test data accuracy)\n");
			System.out.print("5: Iris  (traing accuracy)\n");
			System.out.print("6: Iris  (Test data accuracy)\n");
			System.out.print("7: Iris Noisy (0% - 20%)\n");
			System.out.print("8: Iris Noisy (0% - 20%) Using Validation Set\n");
			System.out.print("9: EXIT\n");
			
			int i=scanner.nextInt();
			if(i == 1)
			{
				//attr_amount = fileOper.readAttribute(fn_id_attr);
				ht = fileOper.readDataset(fn_id_train);
				NeuroNetwork neuroNet_identity_3 = new NeuroNetwork(w1,w2,0.2f,8,8,3,0);
				neuroNet_identity_3.setMomentum(0.00011f);
				neuroNet_identity_3.backpropagation(ht,6600);
				neuroNet_identity_3.testOutput(ht,false);
			}
			else if(i == 2)
			{
				attr_amount = fileOper.readAttribute(fn_id_attr);
				ht = fileOper.readDataset(fn_id_train);
				// 4 hidden units for "identity" training
				NeuroNetwork neuroNet_identity_4 = new NeuroNetwork(w3,w4,0.1f,8,8,4,0);
				neuroNet_identity_4.setMomentum(0.00011f);
				neuroNet_identity_4.backpropagation(ht,6600);
				neuroNet_identity_4.testOutput(ht,false);
			}
			else if(i == 3)
			{
				ht = fileOper.readTennisData(fn_ten_attr, fn_ten_train);
				// tennis //3 hidden units, 1 output
				NeuroNetwork neuroNet_tennis = new NeuroNetwork(w7,w8,0.2f,4,1,3,0);
				neuroNet_tennis.setMomentum(0.00011f);
				neuroNet_tennis.backpropagation(ht,300);
				neuroNet_tennis.testOutput(ht,false);
			}
			else if(i == 4)
			{
				ht = fileOper.readTennisData(fn_ten_attr, fn_ten_train);
				// tennis //3 hidden units, 1 output
				NeuroNetwork neuroNet_tennis = new NeuroNetwork(w7,w8,0.1f,4,1,3,0);
				neuroNet_tennis.setMomentum(0.00011f);
				neuroNet_tennis.backpropagation(ht,300);
				
				ht = fileOper.readTennisData(fn_ten_attr,fn_ten_test);
				neuroNet_tennis.testOutput(ht,false);
			}
			else if(i == 5)
			{
				
				ht = fileOper.readIrisData(fn_iris_train);
				// tennis //3 hidden units, 1 output
				// iris //4 hidden units, 1 output
				NeuroNetwork neuroNet_iris = new NeuroNetwork(w9,w10,0.5f,4,3,6,0); //learning rate can't be 0.1, otherwise, all the output will always be 0.5
				
				neuroNet_iris.setMomentum(0.00011f);
				neuroNet_iris.backpropagation(ht,6600);
				
				neuroNet_iris.testOutput(ht,false);
			}
			else if(i == 6)
			{
				
				ht = fileOper.readIrisData(fn_iris_train);
				// tennis //3 hidden units, 1 output
				// iris //4 hidden units, 1 output
				NeuroNetwork neuroNet_iris = new NeuroNetwork(w9,w10,0.5f,4,3,6,0); //learning rate can't be 0.1, otherwise, all the output will always be 0.5
				
				neuroNet_iris.setMomentum(0.00011f);
				neuroNet_iris.backpropagation(ht,6600);
				ht = fileOper.readIrisData(fn_iris_test);
				neuroNet_iris.testOutput(ht,false);
			}
			else if(i == 7)
			{
				
				ht = fileOper.readIrisData(fn_iris_train);
				NeuroNetwork neuroNet_iris = new NeuroNetwork(w9,w10,0.5f,4,3,6,0); //learning rate can't be 0.1, otherwise, all the output will always be 0.5
				//for(int a = 1; a < 10; a++)
				//{
					fileOper.corruptIrisTrainingSet(ht, 0.02f);
					neuroNet_iris.setMomentum(0.00011f);
					neuroNet_iris.backpropagation(ht,6600);
					ht = fileOper.readIrisData(fn_iris_test);
					neuroNet_iris.testOutput(ht,false);
				//}
			}
			else if(i == 8)
			{
				
				ht = fileOper.readIrisData(fn_iris_train);
				NeuroNetwork neuroNet_iris = new NeuroNetwork(w9,w10,0.5f,4,3,6,10); //learning rate can't be 0.1, otherwise, all the output will always be 0.5
				//for(int a = 1; a < 10; a++)
				//{
					fileOper.corruptIrisTrainingSet(ht, 0.02f);
					neuroNet_iris.setMomentum(0.00011f);
					neuroNet_iris.backpropagation(ht,6600);
					ht = fileOper.readIrisData(fn_iris_test);
					neuroNet_iris.testOutput(ht,false);
				//}
			}
			else if(i == 9)
				runningFlag = false;
			
		}
	}
}
