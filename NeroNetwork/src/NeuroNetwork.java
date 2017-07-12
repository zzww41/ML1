import java.util.Hashtable;
import java.util.Iterator;

enum NETWORK_LEVEL
{
	HIDDEN,
	OUTPUT
};

public class NeuroNetwork {
	
	float weights_layer1[][];   // m outputs, every output includes n weights (n input)
	float weights_layer2[][];   
	float weights_layer1_prv[][]; // record the weights of (n - 1)th iterations 
	float weights_layer2_prv[][];

	float learning_rate;
	int in_amount;
	int out_amount;
	int hidden_amount;
	float momentum;
	float enableMomentum;
	int validationSetAmount;
	public NeuroNetwork(float w1[][], float w2[][], float rate, int in, int out, int hidden, int validation_set_amount)
	{
		this.weights_layer1 = w1;
		this.weights_layer2 = w2;
		this.weights_layer1_prv = new float[hidden][in + 1];
		this.weights_layer2_prv = new float[out][hidden + 1];
		copy(w1,this.weights_layer1_prv);
		copy(w2,this.weights_layer2_prv);
		
		this.learning_rate = rate;
		this.in_amount = in;
		this.out_amount = out;
		this.hidden_amount = hidden;
		this.momentum = 0.0f;
		this.validationSetAmount = validation_set_amount;
	}

	public void calHiddenOutput(float input[], float output_hidden[], float weights[][])
	{
		for(int i = 0; i < hidden_amount; i++)
		{
			for(int j = 0; j < in_amount; j++)
			{
				output_hidden[i]= output_hidden[i] + weights[i][j + 1] * input[j];
			}
			float a = weights[i][0];
			output_hidden[i] = output_hidden[i] + weights[i][0];
			output_hidden[i] = (float) (1.0f /(1.0f + (float)Math.exp((-1.0) * output_hidden[i])));
		}
	}
	public void calActual_Output(float input[], float output_hidden[], float output_actual[],float weights[][])
	{
		float tmp = 0;
		for(int i = 0; i < out_amount; i++)
		{
			for(int j = 0; j < hidden_amount; j++)
			{
				tmp = tmp + weights[i][j + 1] * output_hidden[j];
			}
			tmp = tmp + weights[i][0];
			tmp = (float) (1.0f /(1.0f + (float)Math.exp((-1.0f) * tmp)));
			//output_actual[i]=  (float)Math.round(tmp);
			output_actual[i]=  tmp;
			tmp = 0;

		}
	}
	
	public float calError(Hashtable<String,String> ht, float weights_l1[][], float weights_l2[][])
	{

		Iterator<String> it = ht.keySet().iterator();
		float input[] = null;
		float output_hidden[] = null;
		float output_target[] = null;
		float output_actual[] = null;
		int correctCount = 0;
		int totalTestAmount = ht.size();
		float accuracy;
		boolean correct = true;
		float tmp = 0.0f;
		float sum = 0;
		while(it.hasNext())
		{
			output_hidden = new float[this.hidden_amount];
		    output_actual = new float[this.out_amount];
			
			String key = it.next();
			input = StringtoFloatArray(key, this.in_amount);
		
			calHiddenOutput(input,output_hidden,weights_l1);
			//calculate the output of network
			calActual_Output(input,output_hidden, output_actual,weights_l2);

			
			output_target = StringtoFloatArray(ht.get(key),this.out_amount);
			
			for(int i = 0; i < this.out_amount; i++)
			{
				sum = sum + (output_target[i] - output_actual[i])*(output_target[i] - output_actual[i]);
			}
		}
		return sum/2;
			
	}
	
	public int backpropagation(Hashtable<String,String> ht, int numOfIteration)
	{
		
		int index = 1;
		float output_hidden[] = new float[this.hidden_amount];
		float output_actual[] = new float[this.out_amount];
		
		float error_output[] = new float[this.out_amount];;
		float error_hidden[] = new float[this.hidden_amount];
		
		float StoredWeights_l1[][] = new float[this.hidden_amount][this.in_amount + 1];
		float StoredWeights_l2[][] = new float[this.out_amount][this.hidden_amount + 1];
		copy(this.weights_layer1,StoredWeights_l1);
		copy(this.weights_layer2,StoredWeights_l2);
		
		Hashtable<String, String> validationSet = new Hashtable<String, String>();
		
		float input[] = null;
		float output_target[] = null;
		int trainCaseCount = 0;
		float tmp = 0;
		int iteration_count = 0; 
		int numOfCases = ht.size();
		
		
		while(iteration_count <= numOfIteration)
		{
			Iterator<String> it = ht.keySet().iterator();
			iteration_count++;
			trainCaseCount = 0;
			if(iteration_count == 400)
			{
				int oo = 0;
			}

			if(validationSet.size() != 0)
			{
				float errorOftrainedWeight = calError(validationSet,this.weights_layer1,this.weights_layer2);
				float errorOfStoredWeight = calError(validationSet,StoredWeights_l1,StoredWeights_l2);
				if(errorOftrainedWeight >= errorOfStoredWeight + 0.02 && iteration_count > 250)
				{
					copy(StoredWeights_l1, this.weights_layer1);
					copy(StoredWeights_l2, this.weights_layer2);
					break;  //stop iteration if the error increase significantly using trained weight over validation set
				}
				if(errorOfStoredWeight > errorOftrainedWeight)
				{
					copy(this.weights_layer1,StoredWeights_l1);
					copy(this.weights_layer2,StoredWeights_l2);
					//this.weights_layer1 = StoredWeights_l1;
					//this.weights_layer2 = StoredWeights_l2;
				}
			}
			while(it.hasNext())
			{
				//validationSet will be not empty as long as the program iterate the whole data set once
				if(this.validationSetAmount != 0)
				{
					if(trainCaseCount >= numOfCases - this.validationSetAmount) // the last ten data is validation set 
					{
						String k = it.next();
						validationSet.put(k, ht.get(k));
						trainCaseCount++;
						continue;
					}
					
				}
				 output_hidden = new float[this.hidden_amount];
				 output_actual = new float[this.out_amount];
				
				 error_output = new float[this.out_amount];
				 error_hidden = new float[this.hidden_amount];
				 tmp = 0;
				 
				String key = it.next();
				//String key = k1.trim();
				input = StringtoFloatArray(key,this.in_amount);
				// calculate the output of the hidden layer
				calHiddenOutput(input,output_hidden,this.weights_layer1);
				/*for(int i = 0; i < hidden_amount; i++)
				{
					for(int j = 0; j < in_amount; j++)
					{
						output_hidden[i]= output_hidden[i] + weights_layer1[i][j + 1] * input[j];
					}
					float a = weights_layer1[i][0];
					output_hidden[i] = output_hidden[i] + weights_layer1[i][0];
					output_hidden[i] = (float) (1.0f /(1.0f + (float)Math.exp((-1.0) * output_hidden[i])));
				}*/
				//calculate the output of network
				
				calActual_Output(input,output_hidden, output_actual,this.weights_layer2);
				/*
				for(int i = 0; i < out_amount; i++)
				{
					for(int j = 0; j < hidden_amount; j++)
					{
						tmp = tmp + weights_layer2[i][j + 1] * output_hidden[j];
					}
					tmp = tmp + weights_layer2[i][0];
					tmp = (float) (1.0f /(1.0f + (float)Math.exp((-1.0f) * tmp)));
					//output_actual[i]=  (float)Math.round(tmp);
					output_actual[i]=  tmp;
					tmp = 0;
	
				}*/
				output_target = StringtoFloatArray(ht.get(key),this.out_amount);
				//Propagate the errors backward through the network
				
				//For each network output unit k, calculate its error term
	
				for(int k = 0; k < out_amount; k++)
				{
					error_output[k] = output_actual[k] * (1.0f - output_actual[k]) * (output_target[k] - output_actual[k]);
				
					//calculate the error of current weights using on validation set 
				}
				
				tmp = 0;
				for(int a = 0; a < hidden_amount; a++)
				{
					for(int b = 0; b < out_amount; b++)
					{
						tmp = tmp + weights_layer2[b][a + 1] * error_output[b];
					}
					error_hidden[a] = output_hidden[a] * (1.0f - output_hidden[a])* tmp;
				}
				
				updateWeights(error_hidden, input, NETWORK_LEVEL.HIDDEN, weights_layer1, weights_layer1_prv,iteration_count);
				updateWeights(error_output, output_hidden, NETWORK_LEVEL.OUTPUT, weights_layer2, weights_layer2_prv,iteration_count);
	
				trainCaseCount++;
			}
			
			 //output_hidden = null;
			 //output_actual = null;
			 //error_output = null;
			 //error_hidden = null;
		}
		return iteration_count;
	}
	
	public void updateWeights(float error[], float input[], NETWORK_LEVEL network, float weights[][], float weights_prv[][], int iter_count)
	{
		int network_in = 0;
		int network_out = 0;
		float temp = 0;
		if(network == NETWORK_LEVEL.HIDDEN)
		{
			network_in = in_amount;
			network_out = hidden_amount;
		}
		else if(network == NETWORK_LEVEL.OUTPUT)
		{
			network_in = hidden_amount;
			network_out = out_amount;
		}
		//update exclude w0
		for(int i = 0; i < network_out; i++)
		{
			for(int j = 0; j < network_in; j++)
			{
				temp = weights[i][j + 1];
				if(iter_count == 1)
					weights[i][j + 1] = weights[i][j + 1] + (learning_rate * error[i] * input[j]);
				else if(iter_count > 1)
					weights[i][j + 1] = weights[i][j + 1] + (learning_rate * error[i] * input[j]) + momentum * weights_prv[i][j + 1];
				
				weights_prv[i][j + 1] = temp;
			}
		}
	}
	
	public float[] StringtoFloatArray(String str , int elem_amount)
	{
		float result[] = new float[elem_amount];
		String s[];
		s = str.split(" ");
		for(int i = 0; i < s.length; i++)
		{
			
			result[i] = Float.parseFloat(s[i]);
			
		}
		return result;
	}
	
	public int[] FloatToIntArray(float[] num)
	{
		int result[] = new int[8];
		for(int i = 0; i < num.length; i++)
		{
			result[i] = (int)num[i];
			
		}
		return result;
		
	}
	
	public void testOutput(Hashtable<String,String> ht, boolean onlyPrintAccuracy)
	{
		Iterator<String> it = ht.keySet().iterator();
		float input[] = null;
		float output_hidden[] = null;
		float output_target[] = null;
		float output_actual[] = null;
		int correctCount = 0;
		int totalTestAmount = ht.size();
		float accuracy;
		boolean correct = true;
		float tmp = 0.0f;
		while(it.hasNext())
		{
			output_hidden = new float[this.hidden_amount];
		    output_actual = new float[this.out_amount];
			
			String key = it.next();
			input = StringtoFloatArray(key, this.in_amount);
		
			for(int i = 0; i < hidden_amount; i++)
			{
				for(int j = 0; j < in_amount; j++)
				{
					output_hidden[i]= output_hidden[i] + weights_layer1[i][j + 1] * input[j];
				}
				output_hidden[i] = output_hidden[i] + weights_layer1[i][0];
				output_hidden[i] = (float) (1.0f /(1.0f + (float)Math.exp((-1.0) * output_hidden[i])));
			}
			//calculate the output of network
			for(int i = 0; i < out_amount; i++)
			{
				for(int j = 0; j < hidden_amount; j++)
				{
					tmp = tmp + weights_layer2[i][j + 1] * output_hidden[j];
				}
				tmp = tmp + weights_layer2[i][0];
				tmp = (float) (1.0f /(1.0f + (float)Math.exp((-1.0f) * tmp)));
				//output_actual[i]=  (float)Math.round(tmp);
				output_actual[i]=  tmp;
				tmp = 0;
	
			}
			
			output_target = StringtoFloatArray(ht.get(key),this.out_amount);
			if(!onlyPrintAccuracy)
			{
				System.out.print("Input:  ");
				for(int a = 0; a < this.in_amount; a++)
				{
					System.out.print(input[a] + "    ");
					
				}
				System.out.print("\n");
				System.out.print("Hidden:  ");
				for(int a = 0; a < this.hidden_amount; a++)
				{
					System.out.print(output_hidden[a] + "    ");
				}
				System.out.print("\n");
				System.out.print("Output Target:  ");
				//System.out.print(output_hidden);
				correct = true;
				for(int a = 0; a < this.out_amount; a++)
				{
					System.out.print(output_target[a] + "    ");
				}
			}
			for(int a = 0; a < this.out_amount; a++)
			{
				if(!onlyPrintAccuracy)
					System.out.print("Output Actual:" + output_actual[a] + "    ");
				
				float i = output_target[a] - output_actual[a];
				
				if(Math.abs(i) > 0.5) //
					correct = false;
			}
			if(!onlyPrintAccuracy)
				System.out.print("\n\n");
			if(correct)
				correctCount++;
		}
		accuracy = (float)correctCount/(float)totalTestAmount;
		System.out.print("Accuracy:  ");
		System.out.print(accuracy * 100 + "%" + "\n\n");
	}
	
	public void copy(float a[][], float b[][])
	{
	  for (int i = 0; i < b.length; i++) 
	  {                     
            System.arraycopy(a[i], 0, b[i], 0, a[i].length); 
		             // bb[i] = aa[i].clone();
	  }
	}
	
	public void setMomentum(float m)
	{
		this.momentum = m;
	}
	
	public void printWeights(int type)
	{
		float weights[][];
		if(type == 0)
			weights = weights_layer1;
		else
			weights = weights_layer2;
		for(int i = 0; i < weights.length; i++)
		{
			for(int j = 0; j < weights[i].length; j++)
			{
				System.out.print(weights[i][j]);
				System.out.print("  ");
			}
			System.out.print("\n");
		}
	}
}

