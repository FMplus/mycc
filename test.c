int main (){
        int score[6] = {76, 82, 90, 86, 79, 62};
        int credit[6] = {2, 2, 1, 2, 2, 3};
        int mean, sum, tmp, i;

        temp = 0;
        sum = 0;
        for( i = 0; i < 6; i++)
        {
                sum = sum + score[i] * credit[i];
        }
        
        while(i != 0) {
                tmp = tmp + credit[i - 1];
                i--;
        }
        mean = sum / temp;

        if(mean >= 60){
                mean = mean - 60;
        }
        else{
                mean = 60 - mean;
        }
}